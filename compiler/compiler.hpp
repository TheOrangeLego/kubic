#ifndef _COMPILER_HPP
#define _COMPILER_HPP

#include <fstream>
#include <string>

#include "compiler/assembly.hpp"
#include "parser/node.hpp"
#include "shared/errors.hpp"

static unsigned int labelCounter = 0;

bool nodeTypeMatch( const Node* _node, const NodeType _nodeType ) {
  return _node->getNodeType() == _nodeType;
}

std::string binaryOperator( const Node* _node ) {
  if ( _node->getText() == "+" ) {
    return "add";
  } else if ( _node->getText() == "-" ) {
    return "sub";
  } else if ( _node->getText() == "*" ) {
    return "imul";
  } else if ( _node->getText() == "/" ) {
    return "idiv";
  }

  return "";
}

std::string compile( Node* _node ) {
  std::stringstream representation;

  if ( !_node ) {
    return "";
  } else if ( nodeTypeMatch( _node, NodeType::NodeConstant ) ) {
    representation << insn( "mov", Register::RAX, formatValue( _node ) );
  } else if ( nodeTypeMatch( _node, NodeType::NodeBoolean ) ) {
    representation << insn( "mov", Register::RAX, formatValue( _node ) );
  } else if ( nodeTypeMatch( _node, NodeType::NodeVariable ) ) {
    representation << insn( "mov", Register::RAX, regOffset( Register::RBP, getVariableOffset( _node->getText() ) ) );
  } else if ( nodeTypeMatch( _node, NodeType::NodeBinding ) ) {
    BindingNode* node = (BindingNode*) _node;
    representation << compile( node->getBindingExpression() );
    representation << pushInsn( Register::RAX );
  } else if ( nodeTypeMatch( _node, NodeType::NodeUnaryOperator ) ) {
    /* TODO -- implement once more operators have been decided */
  } else if ( nodeTypeMatch( _node, NodeType::NodeBinaryOperator ) ) {
    BinaryOperatorNode* node = (BinaryOperatorNode*) _node;
    representation << compile( node->getRightOperand() );
    representation << pushInsn( Register::RAX );
    representation << compile( node->getLeftOperand() );
    representation << popInsn( Register::RBX );
    representation << insn( binaryOperator( _node ), Register::RAX, Register::RBX );

    if ( node->getText() == "*" ) {
      representation << insn( "sar", Register::RAX, "2" );
    }
  } else if ( nodeTypeMatch( _node, NodeType::NodeMultiStatement ) ) {
    MultiStatementNode* node = (MultiStatementNode*) _node;
    for ( Node* statement : node->getStatements() ) {
      representation << compile( statement );
    }
  } else if ( nodeTypeMatch( _node, NodeType::NodeConditional ) ) {
    ConditionalNode* node = (ConditionalNode*) _node;
    unsigned int currentCounter = labelCounter++;
    representation << compile( node->getConditional() );
    representation << jumpNotTrue( "else_body", currentCounter );
    representation << compile( node->getUpperBody() );
    representation << jump( "end_if_else", currentCounter );
    representation << label( "else_body", currentCounter );
    representation << compile( node->getLowerBody() );
    representation << label( "end_if_else", currentCounter );
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
    asmFile << "section .text" << std::endl
            << "  global kubic_main" << std::endl
            << std::endl
            << "kubic_main:" << std::endl
            << generatedCode
            << "  ret" << std::endl;
  }
  asmFile.close();
}

#endif
