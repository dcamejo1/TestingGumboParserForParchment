/***************************************************************************
 * 
 * $Id$
 * 
 **************************************************************************/

/**
 * @file $HeadURL$
 * @author $Author$(hoping@baimashi.com)
 * @date $Date$
 * @version $Revision$
 * @brief 
 *  
 **/

#include "Node.h"
#include "Selection.h"
#include "QueryUtil.h"
#include <iostream>

CNode::CNode(GumboNode* apNode)
{
	mpNode = apNode;
}

CNode::~CNode()
{
}

CNode CNode::parent()
{
	return CNode(mpNode->parent);
}

CNode CNode::nextSibling()
{
	return parent().childAt(mpNode->index_within_parent + 1);
}

CNode CNode::prevSibling()
{
	return parent().childAt(mpNode->index_within_parent - 1);
}

unsigned int CNode::childNum()
{
	if (mpNode->type != GUMBO_NODE_ELEMENT)
	{
		return 0;
	}

	return mpNode->v.element.children.length;

}

std::string CNode::outerHTML()
{
	std::string htmlString = "";
	htmlString = outerHTML_converter(mpNode);
	return htmlString;
}

std::string CNode::outerHTML_converter(GumboNode* root)

{
	std::string temp = "";
	if (root == nullptr)
	{
		return temp;
	}
	switch (root->type)
	{
	case GUMBO_NODE_DOCUMENT:
		for (unsigned int i = 0; i < root->v.document.children.length; ++i) {
			temp += outerHTML_converter(static_cast<GumboNode*>(root->v.document.children.data[i]));
		}
		break;
	case GUMBO_NODE_ELEMENT:
		temp += "<" + std::string(gumbo_normalized_tagname(root->v.element.tag));
		GumboAttribute* attr;
		for (unsigned int i = 0; i < root->v.element.attributes.length; ++i) {
			attr = static_cast<GumboAttribute*>(root->v.element.attributes.data[i]);
			temp += " " + std::string(attr->name) + "=\"" + std::string(attr->value) + "\"";
		}
		temp += ">";
		for (unsigned int i = 0; i < root->v.element.children.length; ++i) {
			temp += outerHTML_converter(static_cast<GumboNode*>(root->v.element.children.data[i]));
		}
		temp += "</" + std::string(gumbo_normalized_tagname(root->v.element.tag)) + ">";
		break;
	case GUMBO_NODE_TEXT:
	case GUMBO_NODE_WHITESPACE:
		temp += std::string(root->v.text.text);
		break;
	case GUMBO_NODE_CDATA:
		temp += std::string(root->v.text.text);
		break;
	case GUMBO_NODE_COMMENT:
		temp += std::string(root->v.text.text);
		break;
	default:
		break;
	}

	return temp;
}

std::string CNode::innerHTML()
{
	std::string htmlString = "";
	htmlString = innerHTML_converter(mpNode);
	return htmlString;
}

std::string CNode::innerHTML_converter(GumboNode* root)

{
	recursCount++;
	std::string temp = "";
	if (root == nullptr)
	{
		recursCount--;
		return temp;
	}
	switch (root->type)
	{
	case GUMBO_NODE_DOCUMENT:
		for (unsigned int i = 0; i < root->v.document.children.length; ++i) {
			temp += innerHTML_converter(static_cast<GumboNode*>(root->v.document.children.data[i]));
		}
		break;
	case GUMBO_NODE_ELEMENT:
		if (recursCount > 0)
		{
			temp += "<" + std::string(gumbo_normalized_tagname(root->v.element.tag));
			GumboAttribute* attr;
			for (unsigned int i = 0; i < root->v.element.attributes.length; ++i) {
				attr = static_cast<GumboAttribute*>(root->v.element.attributes.data[i]);
				temp += " " + std::string(attr->name) + "=\"" + std::string(attr->value) + "\"";
			}
			temp += ">";
		}
		for (unsigned int i = 0; i < root->v.element.children.length; ++i) {
			temp += innerHTML_converter(static_cast<GumboNode*>(root->v.element.children.data[i]));
		}
		if (recursCount > 0)
		{
			temp += "</" + std::string(gumbo_normalized_tagname(root->v.element.tag)) + ">";
		}
		break;
	case GUMBO_NODE_TEXT:
	case GUMBO_NODE_WHITESPACE:
		temp += std::string(root->v.text.text);
		break;
	case GUMBO_NODE_CDATA:
		temp += std::string(root->v.text.text);
		break;
	case GUMBO_NODE_COMMENT:
		temp += std::string(root->v.text.text);
		break;
	default:
		break;
	}

	recursCount--;
	return temp;
}

void CNode::removeNode(GumboNode* target, GumboNode* root)
{
	for (int i = 0; i < root->v.document.children.length; i++)
	{
		if (root->v.document.children.data[i] == target)
		{
			memmove(root->v.document.children.data[i],
					root->v.document.children.data[i + 1]),
					sizeof(GumboNode*) * (root->v.document.children.length - i - 1));
			root->v.document.children.length--;
			for (int j = 0; j < target->v.document.children.length; j++)
			{
				(root->v.document.children.data[j])->parent
			}
		}
	}

}

bool CNode::valid()
{
	return mpNode != NULL;
}

CNode CNode::childAt(size_t i)
{
	if (mpNode->type != GUMBO_NODE_ELEMENT || i >= mpNode->v.element.children.length)
	{
		return CNode();
	}

	return CNode((GumboNode*) mpNode->v.element.children.data[i]);
}

std::string CNode::attribute(std::string key)
{
	if (mpNode->type != GUMBO_NODE_ELEMENT)
	{
		return "";
	}

	GumboVector attributes = mpNode->v.element.attributes;
	for (unsigned int i = 0; i < attributes.length; i++)
	{
		GumboAttribute* attr = (GumboAttribute*) attributes.data[i];
		if (key == attr->name)
		{
			return attr->value;
		}
	}

	return "";
}

std::string CNode::text()
{
	return CQueryUtil::nodeText(mpNode);
}

std::string CNode::ownText()
{
	return CQueryUtil::nodeOwnText(mpNode);
}

size_t CNode::startTagRight()
{
	switch(mpNode->type)
	{
	  case GUMBO_NODE_ELEMENT:
		  return mpNode->v.element.start_pos.offset + mpNode->v.element.original_tag.length;
	  case GUMBO_NODE_TEXT:
		  return mpNode->v.text.start_pos.offset;
	  default:
		  return 0;
  }
}

size_t CNode::endTagLeft()
{
	switch(mpNode->type)
	{
	  case GUMBO_NODE_ELEMENT:
		  return mpNode->v.element.end_pos.offset;
	  case GUMBO_NODE_TEXT:
		  return mpNode->v.text.original_text.length + startTagRight();
	  default:
		  return 0;
	}
}

size_t CNode::startTagLeft()
{
	switch(mpNode->type)
	{
	case GUMBO_NODE_ELEMENT:
		return mpNode->v.element.start_pos.offset;
	case GUMBO_NODE_TEXT:
		return mpNode->v.text.start_pos.offset;
	default:
		return 0;
	}
}

size_t CNode::endTagRight()
{
	switch(mpNode->type)
	{
	case GUMBO_NODE_ELEMENT:
		return mpNode->v.element.end_pos.offset + mpNode->v.element.original_end_tag.length;
	case GUMBO_NODE_TEXT:
		return mpNode->v.text.original_text.length + startTagRight();
	default:
		return 0;
	}
}

size_t CNode::length()
{
	return endTagRight() - startTagLeft();
}

std::string CNode::tag()
{
	if (mpNode->type != GUMBO_NODE_ELEMENT)
	{
		return "";
	}

	return gumbo_normalized_tagname(mpNode->v.element.tag);
}

CSelection CNode::find(std::string aSelector)
{
	CSelection c(mpNode);
	return c.find(aSelector);
}