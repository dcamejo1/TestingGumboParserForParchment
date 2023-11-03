#include "Document.h"

CDocument::CDocument()
{
	parsed = NULL;
}

void CDocument::parse(const std::string& htmlString)
{
	reset();
	parsed = gumbo_parse(htmlString.c_str());
}

CDocument::~CDocument()
{
	reset();
}

GumboNode* CDocument::getParsedRoot()
{
    return parsed->root;
}

std::string CDocument::outer_text()
{
    std::string htmlString = "";
    htmlString = outer_text_converter(parsed->root);
    return htmlString;
}

std::string CDocument::outer_text_converter(GumboNode* root)
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
            temp += outer_text_converter(static_cast<GumboNode*>(root->v.document.children.data[i]));
        }
        break;
    case GUMBO_NODE_ELEMENT:
        temp += "<" + std::string(gumbo_normalized_tagname(root->v.element.tag));
        GumboAttribute* attr;
        for (unsigned int i = 0; i < root->v.element.attributes.length; ++i)
        {
            attr = static_cast<GumboAttribute*>(root->v.element.attributes.data[i]);
            temp += " " + std::string(attr->name) + "=\"" + std::string(attr->value) + "\"";
        }
        temp += ">";
        for (unsigned int i = 0; i < root->v.element.children.length; ++i) {
            temp += outer_text_converter(static_cast<GumboNode*>(root->v.element.children.data[i]));
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

std::string CDocument::body_text()
{
    std::string htmlString = "";
    htmlString = body_text_converter(parsed->root);
    return htmlString;
}

std::string CDocument::body_text_converter(GumboNode* root)
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
            temp += body_text_converter(static_cast<GumboNode*>(root->v.document.children.data[i]));
        }
        break;
    case GUMBO_NODE_ELEMENT:
        if (recursCount >= 2)
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
            temp += body_text_converter(static_cast<GumboNode*>(root->v.element.children.data[i]));
        }
        if (recursCount >= 2)
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

void CDocument::remove_gumbo_nodes(GumboNode* node)
{

}

// Compares input GumboNodes to see if they're equivalent. Have to check each field within the node for proper comparison of equivalency.
//bool CDocument::gumbo_node_equals(GumboNode* lhs, GumboNode* rhs)
//{
//    if (lhs == nullptr || rhs == nullptr)
//    {
//        return false;
//    }
//    if (lhs == nullptr && rhs == nullptr)
//    {
//        return true;
//    }
//    if (lhs->index_within_parent != rhs->index_within_parent)
//    {
//        std::cout << "index w/in parent\n\n\n" << std::endl;
//        return false;
//    }   // Checks that both are not nullptr, if one is, this will fail
//    if ((lhs->parent != nullptr) != (rhs->parent != nullptr))
//    {
//        if (!gumbo_node_equals(lhs->parent, rhs->parent))
//        {
//            std::cout << "parent\n\n\n" << std::endl;
//            return false;
//        }
//    }
//    else
//    {
//        std::cout << "one parent was null while the other wasn't\n\n\n" << std::endl;
//        return false;
//    }
//    if (lhs->parse_flags != rhs->parse_flags)
//    {
//        std::cout << "parse_flags\n\n\n" << std::endl;
//        return false;
//    }
//    if (lhs->type != rhs->type)
//    {
//        std::cout << "type\n\n\n" << std::endl;
//        return false;
//    }
//    if (lhs->v.document.children.length != rhs->v.document.children.length)
//    {
//        return false;
//    }
//    for (int i = 0; i < lhs->v.document.children.length; i++)
//    {
//        if (lhs->v.document.children.data[i] != rhs->v.document.children.data[i])
//        {
//            std::cout << "kids' data isn't ==\n\n\n" << std::endl;
//            return false;
//        }
//    }
//    std::cout << "W rizz\n\n\n" << std::endl;
//    return true;
//}

CSelection CDocument::find(std::string selector)
{
	if (parsed == NULL)
	{
		throw "document not initialized";
	}

	CSelection sel(parsed->root);
	return sel.find(selector);
}

void CDocument::reset()
{
	if (parsed != NULL)
	{
		gumbo_destroy_output(&kGumboDefaultOptions, parsed);
		parsed = NULL;
	}
}
