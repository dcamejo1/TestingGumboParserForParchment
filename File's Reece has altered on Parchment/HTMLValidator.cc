#include "HMTLValidator.h"
#include "../utils/utils.h"
using std::cout; using std::endl; using std::string;

// Constructor takes the index file name so it can retrieve the proprietary element lists and store them in the class itself
HTMLValidator::HTMLValidator(std::string boilerplatePath)
{
    this->indexHTML = this->HTML_file_to_string(boilerplatePath);
    CDocument index;
    index.parse(this->indexHTML.c_str());

    CSelection c = index.find("script[id=\"toolElements\"]");
    CNode node = c.nodeAt(0);
    split_string(node.text(), this->toolTags);
}

int HTMLValidator::checkComplete(string html) {
    int startIndex = html.length() - sizeof("</parchment-page></body></html>") - 3000;
    startIndex = startIndex < 0 ? 0 : startIndex;  // Can't be negative!
    if(html.find("</parchment-page>", startIndex) == std::string::npos) {
        std::cout << "  HTML wrapper doesn't have a closing tag within 3000 characters from end." << std::endl;
        return -1;
    }
    return 1;
}

// The main method that cleans up HTML for the Parchment application
std::string HTMLValidator::cleanup_HTML(std::string html, std::string spaceNameFromURL)
{
    if(checkComplete(html) < 0) return "";
    
    CDocument doc;
    doc.parse(html);

    html = get_wrapper(doc, html, spaceNameFromURL);
    doc.parse(html);

    // Unwrap all of the select outlines to remove them but not the element the wrap
    for (auto& tag : this->selectTags) {
        CSelection selectOutlineNodes = doc.find(tag);
        unwrap_all_instances(selectOutlineNodes, html);
        doc.parse(html);
    }

    // Remove the "selected" classes
    CSelection selectedNodes = doc.find(".selected");
    for (int i = selectedNodes.length() -1; i >= 0; i--) {
        remove_class(selectedNodes.nodeAt(i), "selected", html);
    }
    doc.parse(html);

    // Clear the contents of the parchment-pages to only save one layer deep
    CSelection pageNodes = doc.find("#wrapper > parchment-page");
    for (int i = pageNodes.length() - 1; i >= 0; i--) {
        remove_content(pageNodes.nodeAt(i), html);
    }

    // Replace image urls with url for blank-icon
    doc.parse(html);
    CSelection imageNodes = doc.find("image-icon");
    for (int i = imageNodes.length() - 1; i >= 0; i--) {
        replace_backround_image_url(imageNodes.nodeAt(i), "75820185/_res/fileIcons/blank-icon.svg", html);
    }

    // Remove all tool elements
    for (auto& tag : this->toolTags) {
        doc.parse(html);
        CSelection toolNodes = doc.find(tag);
        remove_all_instances(toolNodes, html);
    }

    /*std::cout << "\n\nOuter_text: " << doc.outer_text() << "\n\n" << std::endl;
    std::cout << "\n\nInner_text: " << doc.inner_text() << "\n\n" << std::endl;
    std::cout << "\n\nhtml return string: " << html << "\n\n" << std::endl;*/


    // be able to remove/add style attributes
    //remove additional beginning and ending tags from outerhtml() function for returning to spaceserver
    //remove/add nodes

    return html;
}

void HTMLValidator::getSpaceFiles(std::string& html,  std::vector<string>& folders, std::vector<string>& files) {
    CDocument doc;
    doc.parse(html);
    CSelection selectedFolders = doc.find("#wrapper > parchment-page");
    for (int i = selectedFolders.length() - 1; i >= 0; i--) {
        folders.push_back(selectedFolders.nodeAt(i).attribute("data-name"));
    }
    CSelection selectedFiles = doc.find("#wrapper > file-icon, #wrapper > image-icon");
    for (int i = selectedFiles.length() - 1; i >= 0; i--) {
        files.push_back(selectedFiles.nodeAt(i).attribute("data-name"));
    }
}

// Helper function to split the proprietary element text into their respective vector
void HTMLValidator::split_string(std::string s, std::vector<std::string>& v)
{
    std::istringstream iss(s);
    std::copy(std::istream_iterator<std::string>(iss),
        std::istream_iterator<std::string>(),
        back_inserter(v));
}

// Reads the HTML file into a string
std::string HTMLValidator::HTML_file_to_string(std::string filename)
{
	std::ifstream in(filename, std::ios::in | std::ios::binary);
	if (!in) {
		std::cout << "File " << filename << " not found!\n";
		return "";
	}

	std::string contents;
	in.seekg(0, std::ios::end);
	contents.resize(in.tellg());
	in.seekg(0, std::ios::beg);
	in.read(&contents[0], contents.size());
	in.close();

	return contents;
}

// Retrieves the wrapper element and checks a few validations, returns an empty string if any fail
std::string HTMLValidator::get_wrapper(CDocument& doc, std::string html, std::string spaceNameFromURL)
{
    cout << html << endl;
    CSelection wrapperNodes = doc.find("parchment-page[id=wrapper]");

    if (wrapperNodes.length() > 1) {
        std::cout << "HTML contains more than 1 wrapper." << std::endl;
        for (int i = 0; i < wrapperNodes.length(); i++) {
            std::cout << "wrapper " << i << " name: " << wrapperNodes.nodeAt(i).attribute("data-name") << std::endl;
        }
        return "";
    }
    else if (wrapperNodes.length() == 0) {
        std::cout << "HTML contains no wrapper." << std::endl;
        return "";
    }

    CNode wrapperNode = wrapperNodes.nodeAt(0);
    CNode titleNode = doc.find("title").nodeAt(0);

    if (wrapperNode.attribute("data-name") != titleNode.ownText()) {
        std::cout << "HTML wrapper name does not match document title." << std::endl;
        std::cout << "wrapper name: " << wrapperNode.attribute("data-name") << " | title: " << titleNode.ownText() << std::endl;
        return "";
    }
    
    if (wrapperNode.attribute("data-name") != spaceNameFromURL) {
        std::cout << "HTML wrapper name does not match space name from URL." << std::endl;
        std::cout << "wrapper name: " << wrapperNode.attribute("data-name") << " | url: " << spaceNameFromURL << std::endl;
        return "";
    }

    std::string wrapper = html.substr(wrapperNode.startTagLeft(), wrapperNode.endTagRight() - wrapperNode.startTagLeft());
    //bool temp = doc.gumbo_node_equals(wrapperNodes.gumobonodeAt(0), doc.getParsedRoot());
    cout << "wrapperNode Outer: " << wrapperNode.outerHTML() << "\n\n\n";
    cout << "wrapperNode Inner: " << wrapperNode.innerHTML() << "\n\n\n";
   // cout << "wrapper: " << wrapper << "\n" << temp << endl;
    return wrapper;
}

// Returns the element as a string, including its tags
std::string HTMLValidator::get_element_string(CNode node, std::string html)
{
    size_t start = node.startTagLeft(), length;
    
    if (node.startTagLeft() == node.endTagRight()) {
        // Elements with no end tag
        length = node.startTagRight() - node.startTagLeft();
    }
    else {
        length = node.endTagRight() - node.startTagLeft();
    }

    return html.substr(start, length);
}

// Returns the element as a string, excluding its tags
std::string HTMLValidator::get_element_contents(CNode node, std::string html)
{
    size_t start = node.startTagLeft(), length;
    
    if (node.startTagLeft() == node.endTagRight()) {
        // Elements with no end tag
        start = node.startTagLeft();
        length = node.startTagRight() - node.startTagLeft();
    }
    else {
        start = node.startTagRight();
        length = node.endTagLeft() - node.startTagRight();
    }

    return html.substr(start, length);
}

// Removes an element from the html string
void HTMLValidator::remove(CNode node, std::string& html)
{
    size_t start;
    size_t sectionLength;
    if (node.startTagLeft() == node.endTagRight()) {
        // Elements with no end tag
        start = node.startTagLeft();
        sectionLength = node.startTagRight() - node.startTagLeft();
    }
    else {
        start = node.startTagLeft();
        sectionLength = node.endTagRight() - node.startTagLeft();
    }

    html.erase(start, sectionLength);

    while (html[sectionLength] == '\n') {
        html.erase(sectionLength, 1);
    }

    while (html[start] == '\n') {
        html.erase(start, 1);
    }
}

void HTMLValidator::remove_all_instances(CSelection selection, std::string& html)
{
    for (int i = selection.length() - 1; i >= 0; i--) {
        remove(selection.nodeAt(i), html);
    }
}

// Removes the content of an element from the html string
void HTMLValidator::remove_content(CNode node, std::string& html)
{
    size_t start = node.startTagRight();
    size_t length = node.endTagLeft() - node.startTagRight();
    html.erase(start, length);
}

// Replaces the content of an element with the newContent string
void HTMLValidator::replace_content(CNode node, std::string& html, std::string newContent) {
    size_t start = node.startTagRight();
    size_t length = node.endTagLeft() - node.startTagRight();
    html.replace(start, length, newContent);
}

// Removes a class from an element in the html string
void HTMLValidator::remove_class(CNode node, std::string classname, std::string& html)
{
    size_t classStart = html.find("class=", node.startTagLeft()) + 5;
    size_t found = html.find(classname, classStart);
    if (found!=std::string::npos) {
        html.erase(found, classname.length());
    }
    else {
        std::cout << "failed to remove \'" << classname << "\' from " << "|" << get_element_string(node, html) << "|" << std::endl;
    }
}

void HTMLValidator::replace_backround_image_url(CNode node, std::string newUrl, std::string& html) {
    size_t urlStart = html.find("url(&quot;", node.startTagLeft());
    if (urlStart == std::string::npos) {
        std::cout << RED << "Failed to replace url for an image" << END << std::endl;
        return;
    }
    urlStart += 10;
    size_t urlEnd = html.find("&quot;)", urlStart);
    if (urlEnd == std::string::npos) {
        std::cout << RED << "Failed to replace url for an image" << END << std::endl;
        return;
    }
    html.replace(urlStart, urlEnd - urlStart, newUrl);
}

// Removes the tags of an element in the html string, content remains intact
void HTMLValidator::unwrap(CNode node, std::string& html)
{
    // Remove end tag
    html.erase(node.endTagLeft(), node.endTagRight() - node.endTagLeft());
    while (html[node.endTagLeft()] == '\n') {
        html.erase(node.endTagLeft(), 1);
    }

    // Remove start tag
    html.erase(node.startTagLeft(), node.startTagRight() - node.startTagLeft());
    while (html[node.startTagLeft()] == '\n') {
        html.erase(node.startTagLeft(), 1);
    }

    // Should eventually find a way to shift the elements back a level when printed
    //  Tried removing \t's but it didn't seem to do anything, the formatting seems to be weird from the original HTML files
}

void HTMLValidator::unwrap_all_instances(CSelection selection, std::string& html)
{
    for (int i = selection.length() - 1; i >= 0; i--) {
        unwrap(selection.nodeAt(i), html);
    }
}

void HTMLValidator::add_attribute(CNode node, std::string& html, std::string attributeName, std::string attributeValue)
{
    size_t tagEdge = html.find_first_of('>', node.startTagLeft());
    std::string attr = ' ' + attributeName + '=' + '"' + attributeValue + '"';
    html.insert(tagEdge, attr);
}


std::string HTMLValidator::get_HTML_from_file(std::string htmlfilepath, std::string foldername) {
    std::string html = this->indexHTML;
    std::string fromFile = HTML_file_to_string(htmlfilepath);

    CDocument doc;
    doc.parse(html);

    CNode bodyNode = doc.find("body").nodeAt(0);
    replace_content(bodyNode, html, fromFile);

    CNode titleNode = doc.find("title").nodeAt(0);
    replace_content(titleNode, html, foldername);

    html.erase(std::remove(html.begin(), html.end() - 1, '\0'), html.end() - 1);

    return html;
}

std::string HTMLValidator::create_new_html(std::string foldername) {
    std::string html = this->indexHTML;
    
    CDocument doc;
    doc.parse(html);

    CNode titleNode = doc.find("title").nodeAt(0);
    replace_content(titleNode, html, foldername);

    CNode wrapperNode = doc.find("parchment-page[id=wrapper]").nodeAt(0);
    add_attribute(wrapperNode, html, "data-name", foldername);

    html.erase(std::remove(html.begin(), html.end() - 1, '\0'), html.end() - 1);

    return html;
}

// This is basically just for testing purposes to make sure the cleanup method is removing everything it should
bool HTMLValidator::is_valid_html(std::string html)
{
    CDocument doc;
    doc.parse(html);

    // Check if any selectoutline elements are present
    for (auto& tag : this->selectTags) {
        CSelection selectOutlineNodes = doc.find(tag);
        if (selectOutlineNodes.length() > 0) {
            std::cout << "HTML contains " << selectOutlineNodes.length() << " instances of <" << tag << "> elements" << std::endl;
            return false;
        }
    }

    // Check if there are any selected classes 
    CSelection selectedNodes = doc.find(".selected");
    if (selectedNodes.length() > 0) {
        std::cout << "HTML contains instances " << selectedNodes.length() << " of elements with a \".selected\" class" << std::endl;
        return false;
    }

    // Check if any tool elements are present
    for (auto& tag : this->toolTags) {
        CSelection toolNodes = doc.find(tag);
        if (toolNodes.length() > 0) {
            std::cout << "HTML contains " << toolNodes.length() << " instances of <" << tag << "> elements" << std::endl;
            return false;
        }
    }

    // Check that the HTML contains a valid wrapper
    CSelection wrapperNodes = doc.find("parchment-page[id=wrapper]");
    if (wrapperNodes.length() != 1) {
        std::cout << "HTML contains " << wrapperNodes.length() << "wrappers" << std::endl;
        return "";
    }

    return true;
}