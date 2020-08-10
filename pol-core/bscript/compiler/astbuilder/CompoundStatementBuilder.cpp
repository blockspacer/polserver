#include "CompoundStatementBuilder.h"

using EscriptGrammar::EscriptParser;

namespace Pol::Bscript::Compiler
{
CompoundStatementBuilder::CompoundStatementBuilder(
    const SourceFileIdentifier& source_file_identifier, BuilderWorkspace& workspace )
    : SimpleStatementBuilder( source_file_identifier, workspace )
{
}

}  // namespace Pol::Bscript::Compiler