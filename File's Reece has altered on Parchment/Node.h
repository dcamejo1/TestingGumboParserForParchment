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

#ifndef NODE_H_
#define NODE_H_

#include "html/gumbo/gumbo.h"
#include <string>

class CSelection;

class CNode
{
	public:

		CNode(GumboNode* apNode = NULL);

		virtual ~CNode();

	public:

		bool valid();
		CNode parent();
		CNode nextSibling();
		CNode prevSibling();
		unsigned int childNum();
		CNode childAt(size_t i);

		std::string attribute(std::string key);
		std::string text();
		std::string ownText();

		size_t startTagLeft();
		size_t startTagRight();
		size_t endTagLeft();
		size_t endTagRight();
		size_t length();

		std::string outerHTML();
		std::string outerHTML_converter(GumboNode* root);
		std::string innerHTML();
		std::string innerHTML_converter(GumboNode* root);
		// add add/remove node
		void removeNode(GumboNode* target, GumboNode* root);
		// Add setAttri()
		// Add removeAttri()
		// Add/remove style attributes

		std::string tag();
		CSelection find(std::string aSelector);

	private:

		int recursCount = -1;

		GumboNode* mpNode;
};

#endif /* NODE_H_ */

/* vim: set ts=4 sw=4 sts=4 tw=100 noet: */
