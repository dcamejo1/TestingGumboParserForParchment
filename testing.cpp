#include<vector>
#include<iostream>
#include<map>
#include<stack>
#include<string>
#include<algorithm>
#include<gumbo.h>

using namespace std;

// Pretty prints out the HTML stored in GumboNode 
void printNode(GumboNode* node, int depth = 0) {
    std::string padding(depth * 2, ' ');  // 2 spaces per depth level

    switch (node->type) {
        case GUMBO_NODE_DOCUMENT:
            std::cout << padding << "Document" << std::endl;
            break;
        case GUMBO_NODE_ELEMENT:
            std::cout << padding << "Element: " << gumbo_normalized_tagname(node->v.element.tag) << std::endl;
            break;
        case GUMBO_NODE_TEXT:
        case GUMBO_NODE_WHITESPACE:
            std::cout << padding << "Text: " << node->v.text.text << std::endl;
            break;
        case GUMBO_NODE_CDATA:
            std::cout << padding << "CDATA: " << node->v.text.text << std::endl;
            break;
        case GUMBO_NODE_COMMENT:
            std::cout << padding << "Comment: " << node->v.text.text << std::endl;
            break;
        default:
            std::cout << padding << "Unknown node type: " << node->type << std::endl;
            break;
    }

    if (node->type == GUMBO_NODE_ELEMENT) {
        const GumboVector& children = node->v.element.children;
        for (unsigned int i = 0; i < children.length; ++i) {
            printNode(static_cast<GumboNode*>(children.data[i]), depth + 1);
        }
    }
}

// Takes in a root Gumbo Node and returns the HTML it stores as a string
std::string nodeToHTML(GumboNode* node) {
    std::string html;

    if (!node) {
        return html;
    }

    switch (node->type) {
        case GUMBO_NODE_DOCUMENT:
            for (unsigned int i = 0; i < node->v.document.children.length; ++i) {
                html += nodeToHTML(static_cast<GumboNode*>(node->v.document.children.data[i]));
            }
            break;

        case GUMBO_NODE_ELEMENT:
            html += "<" + std::string(gumbo_normalized_tagname(node->v.element.tag));
            GumboAttribute* attr;
            for (unsigned int i = 0; i < node->v.element.attributes.length; ++i) {
                attr = static_cast<GumboAttribute*>(node->v.element.attributes.data[i]);
                html += " " + std::string(attr->name) + "=\"" + std::string(attr->value) + "\"";
            }
            html += ">";
            for (unsigned int i = 0; i < node->v.element.children.length; ++i) {
                html += nodeToHTML(static_cast<GumboNode*>(node->v.element.children.data[i]));
            }
            html += "</" + std::string(gumbo_normalized_tagname(node->v.element.tag)) + ">";
            break;

        case GUMBO_NODE_TEXT:
        case GUMBO_NODE_WHITESPACE:
            html += std::string(node->v.text.text);
            break;

        case GUMBO_NODE_CDATA:
            html += "<![CDATA[" + std::string(node->v.text.text) + "]]>";
            break;

        case GUMBO_NODE_COMMENT:
            html += "<!--" + std::string(node->v.text.text) + "-->";
            break;

        default:
            break;
    }

    return html;
}

// Testing for nodeToHTML Function
bool testNodeToHTMLNoDiff(const std::string& html) {
    GumboOutput* output = gumbo_parse(html.c_str());
    std::string convertedHtml = nodeToHTML(output->root);
    gumbo_destroy_output(&kGumboDefaultOptions, output);
    
    // Compare the two strings
    bool isEqual = (html == convertedHtml);
    if (!isEqual) {
        std::cout << "Failed Test:\n" << html << "\n\nGenerated:\n" << convertedHtml << "\n";
    }
    return isEqual;
}
bool testNodeToHTMLWithDiff(const std::string& html) {
    GumboOutput* output = gumbo_parse(html.c_str());
    std::string convertedHtml = nodeToHTML(output->root);
    gumbo_destroy_output(&kGumboDefaultOptions, output);
    
    bool isEqual = (html == convertedHtml);
    if (!isEqual) {
        const int CHUNK_SIZE = 50; // You can adjust this size
        for (size_t i = 0; i < std::min(html.size(), convertedHtml.size()); i += CHUNK_SIZE) {
            std::string originalChunk = html.substr(i, CHUNK_SIZE);
            std::string generatedChunk = convertedHtml.substr(i, CHUNK_SIZE);

            if (originalChunk != generatedChunk) {
                std::cout << "Difference found starting at position " << i << ":\n";
                std::cout << "Original:   " << originalChunk << "\n";
                std::cout << "Generated:  " << generatedChunk << "\n\n";
            }
        }

        if (html.size() > convertedHtml.size()) {
            std::cout << "Extra content in original:\n" << html.substr(convertedHtml.size()) << "\n";
        } else if (convertedHtml.size() > html.size()) {
            std::cout << "Extra content in generated:\n" << convertedHtml.substr(html.size()) << "\n";
        }
    }
    return isEqual;
}


// Runs every test together 
void runTests() {
    // Test Cases
    // const char* basicHTML = "<html><head><title>Simple Page</title></head><body><h1>Hello, world!</h1></body></html>";
    const char* basicHTML = "<html><head><meta charset=\"UTF-8\"><title>Simple Page</title></head><body><h1>Hello, world!</h1></body></html>";


    std::vector<std::string> testCases = {
        basicHTML,
    };

    int passCount = 0;
    int totalTests = testCases.size();

    for (const auto& testCase : testCases) {
        if (testNodeToHTMLWithDiff(testCase)) {
            ++passCount;
        }
    }

    std::cout << "Passed " << passCount << " out of " << totalTests << " tests.\n";
}


int main(){
    // Space for when printing
    cout << "\n\n" << endl;


    // Space for testing a single HTML string
    
    // Let's take in some HTML
    // const char* basicHtml = "<html><head><title>Simple Page</title></head><body><h1>Hello, world!</h1></body></html>";
    const char* basicHtml = "<!DOCTYPE html><html><head><meta charset=\"UTF-8\"><title>Simple Page</title></head><body><h1>Hello, world!</h1></body></html>";

    // Let's parse it into Gumbo
    GumboOutput* output = gumbo_parse(basicHtml);

    // Let's make sure that we receive the same string we gave it 
    std::cout << "Reconstructed HTML:" << std::endl;
    std::cout << nodeToHTML(output->root) << std::endl;

    cout << "\n\nRunning tests" << endl;
    
    
    runTests();

    // Space for when printing
    cout << "\n\n" << endl;
    return 0;
}

/*
void printTree(GumboNode* node, int depth = 0) {
    if (node->type == GUMBO_NODE_TEXT) {
        for (int i = 0; i < depth; i++) std::cout << "  ";
        std::cout << "Text: " << node->v.text.text << std::endl;
        return;
    } else if (node->type == GUMBO_NODE_ELEMENT || node->type == GUMBO_NODE_TEMPLATE) {
        for (int i = 0; i < depth; i++) std::cout << "  ";
        std::cout << "Element: " << gumbo_normalized_tagname(node->v.element.tag) << std::endl;
        GumboVector* children = &node->v.element.children;
        for (unsigned int i = 0; i < children->length; ++i) {
            printTree(static_cast<GumboNode*>(children->data[i]), depth + 1);
        }
    }
}
*/