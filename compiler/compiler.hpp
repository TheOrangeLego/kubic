#ifndef _COMPILER_HPP
#define _COMPILER_HPP

#include <boost/range/adaptors.hpp>
#include <fstream>
#include <set>
#include <string>

#include "compiler/assembly.hpp"
#include "parser/node.hpp"
#include "shared/errors.hpp"

std::set<std::string> EXTERNAL_FUNCTIONS = {
  "print",
};

static unsigned int labelCounter = 0;

bool nodeTypeMatch( const Node* _node, const NodeType _nodeType ) {
  return _node->getNodeType() == _nodeType;
}

std::string binaryOperator( const Node* _node ) {
  return mapping( BINARY_OPERATOR_ASM, _node->getText(), std::string( "" ) );
}

std::string compile( Node* _node );

void compile( std::stringstream& _representation, const BooleanNode* _node ) {
  _representation << insn( "mov", Register::RAX, formatValue( _node ) );
}

void compile( std::stringstream& _representation, const ConstantNode* _node ) {
  _representation << insn( "mov", Register::RAX, formatValue( _node ) );
}

void compile( std::stringstream& _representation, const VariableNode* _node ) {
  _representation << insn( "mov", Register::RAX, regOffset( Register::RBP, getVariableOffset( _node->getText() ) ) );
}

void compile( std::stringstream& _representation, const BindingNode* _node ) {
  _representation << compile( _node->getBindingExpression() )
                  << pushInsn( Register::RAX );
}

void compile( std::stringstream& _representation, const BinaryOperatorNode* _node ) {
    _representation << compile( _node->getRightOperand() )
                    << pushInsn( Register::RAX )
                    << compile( _node->getLeftOperand() )
                    << popInsn( Register::RBX );

    if ( _node->getValueType() == ValueType::ValueBoolean ) {
      unsigned int currentCounter = labelCounter++;

      _representation << insn( "cmp", Register::RAX, Register::RBX )
                      << jumpInsn( binaryOperator( _node ), "conditional", currentCounter )
                      << insn( "mov", Register::RAX, ASM_FALSE )
                      << jumpInsn( "jmp", "conditional_end", currentCounter )
                      << label( "conditional", currentCounter )
                      << insn( "mov", Register::RAX, ASM_TRUE )
                      << label( "conditional_end", currentCounter );
    } else {
      _representation << insn( binaryOperator( _node ), Register::RAX, Register::RBX );

      if ( _node->getText() == "*" ) {
        _representation << insn( "sar", Register::RAX, "1" );
      }
    }
}

void compile( std::stringstream& _representation, const MultiStatementNode* _node ) {
  for ( Node* statement : _node->getStatements() ) {
    _representation << compile( statement );
  }
}

void compile( std::stringstream& _representation, const ConditionalNode* _node ) {
  unsigned int currentCounter = labelCounter++;

  _representation << compile( _node->getConditional() )
                  << insn( "cmp", Register::RAX, ASM_TRUE )
                  << jumpInsn( "jne", "else_body", currentCounter )
                  << compile( _node->getUpperBody() )
                  << jumpInsn( "jmp", "end_if_else", currentCounter )
                  << label( "else_body", currentCounter )
                  << compile( _node->getLowerBody() )
                  << label( "end_if_else", currentCounter );
}

void compile( std::stringstream& _representation, const FunctionCallNode* _node ) {
    std::vector<Node*> arguments = _node->getArguments();

    for ( Node* argument : boost::adaptors::reverse( arguments ) ) {
      _representation << compile( argument );
      /* TODO -- order of function arguments RDI, RSI, RDX, RCX, R8, R9 */
      _representation << insn( "mov", Register::RDI, Register::RAX );
    }

    _representation << callInsn( _node->getName() );
    /* TODO -- only move RBP if using stack */
}

std::string compile( Node* _node ) {
  std::stringstream representation;

  if ( !_node ) {
    return "";
  } else if ( nodeTypeMatch( _node, NodeType::NodeConstant ) ) {
    compile( representation, (ConstantNode*) _node );
  } else if ( nodeTypeMatch( _node, NodeType::NodeBoolean ) ) {
    compile( representation, (BooleanNode*) _node );
  } else if ( nodeTypeMatch( _node, NodeType::NodeVariable ) ) {
    compile( representation, (VariableNode*) _node );
  } else if ( nodeTypeMatch( _node, NodeType::NodeBinding ) ) {
    compile( representation, (BindingNode*) _node );
  } else if ( nodeTypeMatch( _node, NodeType::NodeUnaryOperator ) ) {
    /* TODO -- implement once more operators have been decided */
  } else if ( nodeTypeMatch( _node, NodeType::NodeBinaryOperator ) ) {
    compile( representation, (BinaryOperatorNode*) _node );
  } else if ( nodeTypeMatch( _node, NodeType::NodeMultiStatement ) ) {
    compile( representation, (MultiStatementNode*) _node );
  } else if ( nodeTypeMatch( _node, NodeType::NodeConditional ) ) {
    compile( representation, (ConditionalNode*) _node );
  } else if ( nodeTypeMatch( _node, NodeType::NodeFunctionCall ) ) {
    compile( representation, (FunctionCallNode*) _node );
  } else {
    return "";
  }

  return representation.str();
}

void compile( Node* _node, const std::string _filename ) {
  std::string asmFilename = _filename + ".ka";

  std::ofstream asmFile( asmFilename );

  std::string generatedCode = compile( _node );

  if ( !emptyErrorsLog() ) {
    printErrors();
  } else {
    asmFile << "section .data" << std::endl;

    for ( std::string function : EXTERNAL_FUNCTIONS ) {
      asmFile << "  extern " << function << std::endl;
    }

    asmFile << std::endl
            << "section .text" << std::endl
            << "  global kubic_main" << std::endl
            << std::endl
            << "kubic_main:" << std::endl
            << generatedCode
            << insn( "mov", Register::RSP, Register::RBP )
            << insn( "sub", Register::RSP, std::to_string( 8 * 5 ) )
            << "  ret" << std::endl;
  }
  asmFile.close();
}

#endif
