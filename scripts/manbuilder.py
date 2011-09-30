#!/usr/bin/python -t

# Copyright (C) 2009 - 2009, Vrai Stacey.
#
# Part of the SimpleFudgeProto distribution.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# Requires at least Python version 2.4 - check before going any further
import sys
assert ( sys.version_info [ : 2 ] >= ( 2, 4 ) )

from xml.dom.minidom import parse as parseXmlFile
from optparse import OptionParser
import os.path, string, textwrap, time

# ==========================================================================
# Utility functions

def _nn ( node ):
    if node.localName:
        return str ( node.localName )

def _an ( node, attrnames ):
    return [ str ( node.getAttribute ( an ) ) for an in attrnames ]

def _partial ( function, *baseargs ):
    def wrapped ( *args ):
        return function ( *( baseargs + args ) )
    return wrapped

def _firstnotof ( haystack, needles ):
    for index in range ( len ( haystack ) ):
        if haystack [ index ] not in needles:
            return index
    return -1


# ==========================================================================
# Container classes - build by the XMLParser, used by the formatters to
# produce the final output

class ManPage ( object ):
    def __init__ ( self,
                   title,
                   section,
                   extra1,
                   extra2,
                   extra3,
                   sections,
                   references,
                   reftable,
                   header ):
        self.__title = title
        self.__section = section
        self.__extra1 = extra1
        self.__extra2 = extra2
        self.__extra3 = extra3
        self.__sections = sections
        self.__references = references
        self.__reftable = reftable
        self.__header = header

    def title ( self ): return self.__title
    def section ( self ): return self.__section
    def extra1 ( self ): return self.__extra1
    def extra2 ( self ): return self.__extra2
    def extra3 ( self ): return self.__extra3
    def sections ( self ): return self.__sections
    def references ( self ): return self.__references
    def reftable ( self ): return self.__reftable
    def header ( self ): return self.__header


class Reference ( object ):
    def __init__ ( self, ident, name, href ):
        self.__ident = ident
        self.__name = name
        self.__href = href
        self.__index = -1

    def ident ( self ): return self.__ident
    def name ( self ): return self.__name
    def href ( self ): return self.__href
    def index ( self ): return self.__index

    def setIndex ( self, index ): self.__index = int ( index )


class Section ( object ):
    def __init__ ( self, name, content ):
        self.__name = name
        self.__content = content

    def name ( self ): return self.__name
    def content ( self ): return self.__content


class TextBlock ( object ):
    def __init__ ( self, text ):
        self.__text = text

    def text ( self ): return self.__text


class StyleBlock ( object ):
    PLAIN  = 0
    BOLD   = 1
    ITALIC = 2
    CODE   = 3
    QUOTED = 4
    INDENT = 5

    flowstyles = ( PLAIN, BOLD, ITALIC, QUOTED )

    def __init__ ( self, style, content ):
        self.__style = style
        self.__content = content

    def style ( self ): return self.__style
    def content ( self ): return self.__content

class BulletPointBlock ( object ):
    def __init__ ( self, content ):
        self.__content = content

    def content ( self ): return self.__content

class BulletListBlock ( object ):
    def __init__ ( self, points ):
        self.__points = points

    def points ( self ): return self.__points

class DefBlock ( object ):
    def __init__ ( self, header, body ):
        self.__header = header
        self.__body = body

    def header ( self ): return self.__header
    def body ( self ): return self.__body


class DefListBlock ( object ):
    def __init__ ( self, definitions ):
        self.__definitions = definitions

    def definitions ( self ): return self.__definitions


class OptionBlock ( object ):
    def __init__ ( self, name, content ):
        self.__name = name
        self.__content = content

    def name ( self ): return self.__name
    def content ( self ): return self.__content


class OptionsBlock ( object ):
    def __init__ ( self, options ):
        self.__options = options

    def options ( self ): return self.__options


class ParagraphBlock ( object ):
    def __init__ ( self, content ):
        self.__content = content

    def content ( self ): return self.__content


class RefBlock ( object ):
    def __init__ ( self, ident, content ):
        self.__ident = ident
        self.__content = content

    def ident ( self ): return self.__ident
    def content ( self ): return self.__content


# Returns true if the block is considered a "flow block", i.e. a block that's
# inline with text rather than a structural block that has other structural
# blocks as siblings.
def _flowblock ( block ):
    return isinstance ( block, RefBlock ) \
           or ( isinstance ( block, StyleBlock )
                and block.style ( ) in StyleBlock.flowstyles )


# ==========================================================================
# Parser for the input file - converts the input XML in to a Section/Block
# tree with a ManPage instance at the root.

class XmlParser:
    _rootChildNodes = ( 'references', 'section', 'header' )

    def __init__ ( self ):
        self.__blockParser = { None        : self._parseText,
                               'bold'      : _partial ( self._parseStyle, StyleBlock.BOLD ),
                               'bltlist'   : self._parseBulletList,
                               'code'      : _partial ( self._parseStyle, StyleBlock.CODE ),
                               'deflist'   : self._parseDefList,
                               'indent'    : _partial ( self._parseStyle, StyleBlock.INDENT ),
                               'italic'    : _partial ( self._parseStyle, StyleBlock.ITALIC ),
                               'options'   : self._parseOptions,
                               'paragraph' : self._parseParagraph,
                               'quote'     : _partial ( self._parseStyle, StyleBlock.QUOTED ),
                               'ref'       : self._parseRef }

        self.__headerParser = { None   : lambda node: str ( node.data ),
                                'meta' : lambda node: '%s%s%s' % ( TagExpander.marker,
                                                                   str ( node.getAttribute ( 'name' ) ).upper ( ),
                                                                   TagExpander.marker ) }

    def parse ( self, stream ):
        document = parseXmlFile ( stream )
        try:
            return self._parseDocument ( document )
        finally:
            document.unlink ( )

    def _parseDocument ( self, document ):
        # Check the the document root node has the correct name
        rootnode = document.documentElement
        if _nn ( rootnode ) != 'manpage':
            raise Exception, 'Document element "%s", not "manpage" as expected' % \
                             _nn ( root )

        # Group the children by interesting node types, maintaining the order
        # for nodes of any one type.
        # grouped = ( ( _nn ( node ), node ) for node in rootnode.childNodes )
        groups = dict ( ( name, [ ] ) for name in self._rootChildNodes )
        for node in rootnode.childNodes:
            group = groups.get ( _nn ( node ) )
            if group is not None: group.append ( node )

        # Construct the lookup table for references and a normalised list of
        # sections
        header     = self._buildHeaderText ( groups [ 'header'] )
        sections   = self._buildSectionList ( groups [ 'section' ] )
        reftable   = self._buildReferenceTable ( groups [ 'references' ] )
        references = self._buildReferenceList ( reftable, groups [ 'references' ] )

        return ManPage ( *( _an ( rootnode, ( 'title',
                                              'section',
                                              'extra1',
                                              'extra2',
                                              'extra3' ) )
                            + [ sections, references, reftable, header ] ) )

    def _buildHeaderText ( self, nodes ):
        total = [ ]
        for node in nodes:
            # Parse the text and other (e.g. meta) tags within the header to get a list
            # of lines with an common indent removed. Unrecognised tags are ignored.
            pairs = [ ( child, self.__headerParser.get ( _nn ( child ) ) )
                      for child in node.childNodes ]
            lines = textwrap.dedent ( ''.join ( handler ( child )
                                                for child, handler in pairs if handler )
                                    ).split ( '\n' )

            for unknown in [ child for child, handler in pairs if not handler ]:
                sys.stderr.write ( 'UNRECOGNISED HEADER TAG "%s"\n' % _nn ( unknown ) )

            if not len ( lines ): continue

            # Remove the leading and/or trailing line if they're only whitespace
            if _firstnotof ( lines [ 0 ], string.whitespace ) < 0:
                lines = lines [ 1 : ]
            if _firstnotof ( lines [ -1 ], string.whitespace ) < 0:
                lines = lines [ : -1 ]

            total += lines
        return '\n'.join ( total )

    def _buildReferenceTable ( self, nodes ):
        # Create a single list of all reference details
        refs = [ Reference ( *_an ( refnode, ( 'id', 'name', 'href' ) ) )
                 for refsnode in nodes
                 for refnode in refsnode.childNodes
                 if _nn ( refnode ) == 'reference' ]

        # Contruct the Id->Reference lookup table while checking that all Ids
        # are unique and populating the numeric index of each Reference.
        table = { }
        index = 1
        for ref in refs:
            if ref.ident ( ) in table:
                raise KeyError, 'Multiple references with id "%s"' % ref.ident ( )
            ref.setIndex ( index )
            index += 1
            table [ ref.ident ( ) ] = ref
        return table

    def _buildSectionList ( self, nodes ):
        return [ self._buildSection ( str ( node.getAttribute ( 'title' ) ),
                                      node.childNodes ) for node in nodes ]

    def _buildSection ( self, title, nodes ):
        return Section ( title, self._parseBlocks ( nodes ) )

    def _buildReferenceList ( self, reftable, nodes ):
        references = [ ]
        for node in nodes:
            references.append ( Section (
                str ( node.getAttribute ( 'title' ) ),
                [ reftable [ str ( child.getAttribute ( 'id' ) ) ]
                  for child in node.childNodes
                  if _nn ( child ) == 'reference' ] ) )
        return references

    def _parseBlocks ( self, nodes ):
         # Filter out null blocks or text blocks that are entirely whitespace
        return [ block for block in map ( self._parseBlock, nodes )
                 if block and not
                   ( isinstance ( block, TextBlock )
                     and _firstnotof ( block.text ( ), string.whitespace ) == -1 ) ]

    def _parseBlock ( self, node ):
        handler = self.__blockParser.get ( _nn ( node ) )
        if handler:
            return handler ( node )
        else:
            sys.stderr.write ( 'UNRECOGNISED TAG "%s"\n' % _nn ( node ) )

    def _parseBulletList ( self, node ):
        points = [ ]
        for child in node.childNodes:
            if _nn ( child ) == 'bltpoint':
                points.append ( BulletPointBlock ( self._parseBlocks ( child.childNodes ) ) )

        if points:
            return BulletListBlock ( points )

    def _parseDefList ( self, node ):
        defs = [ ]
        header = None
        for child in node.childNodes:
            if not header and _nn ( child ) == 'defheader':
                header = self._parseBlocks ( child.childNodes )
            elif header and _nn ( child ) == 'defbody':
                defs.append ( DefBlock ( header,
                                         self._parseBlocks ( child.childNodes ) ) )
                header = None

        if header:
            sys.stderr.write ( 'DEFHEADER WITHOUT DEFBODY\n' )

        if defs:
            return DefListBlock ( defs )

    def _parseMeta ( self, node ):
        return MetaBlock ( str ( node.getAttribute ( 'name' ) ) )

    def _parseOptions ( self, node ):
        options = [ ]
        for child in node.childNodes:
            if _nn ( child ) == 'option':
                options.append ( OptionBlock ( str ( child.getAttribute ( 'name' ) ),
                                               self._parseBlocks ( child.childNodes ) ) )
        if options:
            return OptionsBlock ( options )

    def _parseParagraph ( self, node ):
        return ParagraphBlock ( self._parseBlocks ( node.childNodes ) )

    def _parseRef ( self, node ):
        return RefBlock ( str ( node.getAttribute ( 'id' ) ),
                          self._parseBlocks ( node.childNodes ) )

    def _parseStyle ( self, style, node ):
        return StyleBlock ( style, self._parseBlocks ( node.childNodes ) )

    def _parseText ( self, node ):
        return TextBlock ( str ( node.data ) );


# ==========================================================================
# Tag expander class - replaces tag inserted in to text using <meta> tags

class TagExpander:
    marker = '$$'

    def __init__ ( self ):
        self.__tags = { 'SCRIPTNAME' : lambda: os.path.basename ( sys.argv [ 0 ] ),
                        'TIMESTAMP'  : lambda: time.strftime ( '%Y/%b/%d %H:%M:%SZ', time.gmtime ( ) ) }

    def __call__ ( self, text ):
        elements = text.split ( self.marker )
        if not len ( elements ) % 2:
            raise Exception, 'CANNOT EXPAND TEXT, ODD NUMBER OF EXPANSION MARKERS "%s"' % self.marker
        return ''.join ( self.__replace ( tail = elements ) )

    def __replace ( self, head = [ ], tag = '', tail = [ ] ):
        if tag:
            generator = self.__tags.get ( tag )
            if not generator:
                raise KeyError, 'NO GENERATOR FOR TAG "%s"' % tag
            tag = generator ( )

        if len ( tail ) > 2:
            return self.__replace ( head + [ tag, tail [ 0 ] ], tail [ 1 ], tail [ 2 : ] )
        else:
            return head + [ tag ] + tail


# ==========================================================================
# Formatter classes - takes a ManPage instance and outputs it in the given
# format.

class NroffFormatter:
    class Context ( object ):
        def __init__ ( self, stream, manpage ):
            self.stream = stream
            self.manpage = manpage
            self.index = 0
            self.prevblock = None

        def copy ( self ):
            return self.__class__ ( self.stream, self.manpage )

    def __init__ ( self ):
        self.__tagexpander = TagExpander ( )

        self.__blockFormatter = { BulletListBlock  : self._formatBulletListBlock,
                                  BulletPointBlock : self._formatBulletPointBlock,
                                  DefBlock         : self._formatDefBlock,
                                  DefListBlock     : self._formatDefListBlock,
                                  OptionBlock      : self._formatOptionBlock,
                                  OptionsBlock     : self._formatOptionsBlock,
                                  ParagraphBlock   : self._formatParagraphBlock,
                                  RefBlock         : self._formatRefBlock,
                                  Reference        : self._formatReference,
                                  StyleBlock       : self._formatStyleBlock,
                                  TextBlock        : self._formatTextBlock }

        self.__styleFormatter = { StyleBlock.PLAIN  : _partial ( self._formatSimpleStyleBlock, '', '' ),
                                  StyleBlock.BOLD   : _partial ( self._formatSimpleStyleBlock, '\\fB', '\\fR' ),
                                  StyleBlock.INDENT : self._formatIndentStyleBlock,
                                  StyleBlock.ITALIC : _partial ( self._formatSimpleStyleBlock, '\\fI', '\\fR' ),
                                  StyleBlock.CODE   : self._formatCodeStyleBlock,
                                  StyleBlock.QUOTED : _partial ( self._formatSimpleStyleBlock, '\\(lq', '\\(rq' ), }

    def format ( self, stream, manpage ):
        context = self.Context ( stream, manpage )
        self._formatPageHeader ( context )
        self._formatPageTitle ( context )
        map ( _partial ( self._formatSection, context ), manpage.sections ( ) )
        map ( _partial ( self._formatSection, context ), manpage.references ( ) )

    def _formatPageHeader ( self, context ):
        if context.manpage.header ( ):
            context.stream.write ( ''.join ( '.\\" %s\n' % line for line in
                                             self.__tagexpander ( context.manpage.header ( ) ).split ( '\n' ) ) )
            context.stream.write ( '.\\"\n' )

    def _formatPageTitle ( self, context ):
        args = [ context.manpage.title ( ), context.manpage.section ( ),
                 context.manpage.extra1 ( ), context.manpage.extra2 ( ), context.manpage.extra3 ( ) ]
        context.stream.write ( '.TH %s\n' % ' '.join ( '"%s"' % arg for arg in args if args ) )

    def _formatSection ( self, context, section ):
        context.stream.write ( '.\\"\n.\\" %s\n.\\"\n' % ( '=' * 74 ) )
        context.stream.write ( '.SH "%s"\n' % section.name ( ) )
        self._formatBlockList ( context, section.content ( ) )
        context.stream.write ( '\n' )

    def _formatReference ( self, context, ref ):
        if context.index: context.stream.write ( '\n' )
        context.stream.write ( '.IP "%d." 4\n%s\n.RS 4\n%s\n.RE' % ( ref.index ( ), ref.name ( ), ref.href ( ) ) )

    def _formatBlockList ( self, context, blocks ):
        newcontext = context.copy ( )
        for index in range ( len ( blocks ) ):
            newcontext.index = index
            self._formatBlock ( newcontext, blocks [ index ] )
            newcontext.prevblock = blocks [ index ]

    def _formatBlock ( self, context, block ):
        handler = self.__blockFormatter.get ( type ( block ) )
        if handler:
            handler ( context, block )
        else:
            raise TypeError, 'No formatter in %s for %s' % ( self.__class__.__name__,
                                                             block.__class__.__name__ )

    def _formatBulletPointBlock ( self, context, block ):
        context.stream.write ( '\n.IP \\[bu] 2\n' )
        self._formatBlockList ( context, block.content ( ) )

    def _formatBulletListBlock ( self, context, block ):
        if block.points ( ):
            self._formatBlockList ( context, block.points ( ) )
            context.stream.write ( '\n.P' )

    def _formatDefBlock ( self, context, block ):
        if context.index: context.stream.write ( '\n.sp' )
        context.stream.write ( '\n.IP "' )
        self._formatBlockList ( context, block.header ( ) )
        context.stream.write ( '"\n' )
        self._formatBlockList ( context, block.body ( ) )

    def _formatDefListBlock ( self, context, block ):
        if block.definitions ( ):
            self._formatBlockList ( context, block.definitions ( ) )
            context.stream.write ( '\n.P' )

    def _formatOptionBlock ( self, context, block ):
        if context.index: context.stream.write ( '\n.RE\n' )
        context.stream.write ( '.PP\n\\fB%s\\fR\n.RS 4\n' % self._padText ( block.name ( ) ) )
        self._formatBlockList ( context, block.content ( ) )

    def _formatOptionsBlock ( self, context, block ):
        if block.options ( ):
            self._formatBlockList ( context, block.options ( ) )
            context.stream.write ( '\n.RE 4' )

    def _formatParagraphBlock ( self, context, block ):
        if context.index:
            context.stream.write ( '\n' )

            # Bullet and Definition lists are terminated with a paragraph ending, so
            # don't follow them up with another one.
            if type ( context.prevblock ) not in ( DefListBlock, BulletListBlock ):
                context.stream.write ( '.sp\n' )

        self._formatBlockList ( context, block.content ( ) )

    def _formatRefBlock ( self, context, block ):
        reference = context.manpage.reftable ( ).get ( block.ident ( ) )
        if not reference:
            raise KeyError, 'No reference found for id "%s"' % block.ident ( )

        self._padFlowBlock ( context )
        context.stream.write ( '\\fB' )
        self._formatBlockList ( context, block.content ( ) )
        context.stream.write ( '\\fR[%d]' % reference.index ( ) )

    def _formatStyleBlock ( self, context, block ):
        handler = self.__styleFormatter.get ( block.style ( ) )
        if handler:
            handler ( context, block )
        else:
            raise TypeError, 'No style formatter in %s for %d' % ( self.__class__.__name__,
                                                                   block.style ( ) )

    def _formatSimpleStyleBlock ( self, prefix, suffix, context, block ):
        self._padFlowBlock ( context )
        context.stream.write ( prefix )
        self._formatBlockList ( context, block.content ( ) )
        context.stream.write ( suffix )

    def _formatCodeStyleBlock ( self, context, block ):
        if context.index: context.stream.write ( '\n.sp\n' )
        context.stream.write ( '.nf\n\\f[CR]' )

        # Output code content - text only
        text = ''
        for codeblock in block.content ( ):
            if not isinstance ( codeblock, TextBlock ):
                raise TypeError, 'Only TextBlocks allowed within Code style, not %s' % \
                                 codeblock.__class__.__name__
            text += self._padText ( codeblock.text ( ) )
        context.stream.write ( textwrap.dedent ( text ).strip ( ) )

        context.stream.write ( '\\fR\n.fi' )

    def _formatIndentStyleBlock ( self, context, block ):
        context.stream.write ( '\n.sp\n.RS 4\n' )
        self._formatBlockList ( context, block.content ( ) )
        context.stream.write ( '\n.RE' )

    def _formatTextBlock ( self, context, block ):
        # The leading whitespace is removed from the text, if the previous block was a
        # flow-style and the text originally had leading whitespace; add a single space
        # back in to the output.
        if _flowblock ( context.prevblock ) \
           and _firstnotof ( block.text ( ), string.whitespace ) > 0:
            context.stream.write ( ' ' )

        # If this text is following a non-flow style block, force a newline to be
        # prefixed - this ensures that the end of the style block is not suffixed.
        if isinstance ( context.prevblock, StyleBlock ) \
            and context.prevblock.style ( ) not in StyleBlock.flowstyles:
            context.stream.write ( '\n' )

        lines = [ self._padText ( line.strip ( ) ) for line in block.text ( ).split ( '\n' ) ]
        context.stream.write ( '\n'.join ( line for line in lines if line ) )

    def _padText ( self, text ):
        return text.replace ( '\\', '\\e' ).replace ( '.', '\\.' )

    def _padFlowBlock ( self, context ):
        # As trailing whitespace is removed from text, check to see if the previous
        # has text and if that text originally has trailing whitespace. If so, add a
        # single space before next block.
        if isinstance ( context.prevblock, TextBlock ) \
           and context.prevblock.text ( ) \
           and context.prevblock.text ( ) [ -1 ] in string.whitespace:
            context.stream.write ( ' ' )


# ==========================================================================
#  Script entry point and argument parsing

def _openfile ( filename, default, mode ):
    if filename == '-':
        return default
    try:
        return open ( filename, mode )
    except IOError, exception:
        sys.stderr.write ( 'Failed to open file "%s": %s\n' %
                          ( filename, exception.strerror ) )
        sys.exit ( exception.errno )

def _closefile ( handle ):
    if handle.fileno ( ) > 2:
        try:
            handle.close ( )
        except IOError:
            pass

if __name__ == '__main__':
    formatters = { 'nroff' : NroffFormatter }

    optparser = OptionParser ( description = 'Generates %s manpages from XML definition files.' % '/'.join ( formatters.keys ( ) ) )
    optparser.add_option ( '-i', '--input',  dest = 'input',  help = 'XML input file, or "-" for stdin (default)' )
    optparser.add_option ( '-o', '--output', dest = 'output', help = 'Output file, or "-" for stdout (default)' )
    optparser.add_option ( '-f', '--format', dest = 'format', help = 'Format of the output file: %s' % ', '.join ( formatters.keys ( ) ) )
    optparser.set_defaults ( input = '-', output = '-' )
    options = optparser.parse_args ( ) [ 0 ]

    formatter = formatters.get ( options.format )
    if not formatter:
        optparser.print_help ( )
        sys.exit ( 1 )

    parser = XmlParser ( )
    formatter = formatter ( )

    input = _openfile ( options.input, sys.stdin, 'r' )
    try:
        manpage = parser.parse ( input )
    finally:
        _closefile ( input )

    output = _openfile ( options.output, sys.stdout, 'w' )
    try:
        formatter.format ( output, manpage )
    finally:
        _closefile ( output )


