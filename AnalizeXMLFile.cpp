#include <iostream>
#include<string>
#include<fstream> //file calling

using namespace std;
template <class Z>
class Node
{
public:

    Z data;
    Node<Z>* next;
 
    Node()
    {
        next = nullptr;
        data = nullptr;
    }
    Node(const Z val)
    {
        data = val;
    }
};

//stack implemented here
template <class S>
class Stack
{
private:
    Node<S>* top;

public:
    Stack()
    {
        top = nullptr;
    }

    S Top()
    {
        return top->data;
    }

    bool IsEmpty()
    {
        if (!top)
        {
            return true;
        }
        return false;
    }

    bool push(const S&)
    {
        Node<S>* temp = new Node<S>(value);
        temp->next = top;
        top = temp;
        return true;
    }

    bool pop(S&)
    {
        if (!IsEmpty())
        {
            value = top->data;

            Node<S>* temp = top;
            top = top->next;
            temp->next = nullptr;
            temp = nullptr;
            return true;
        }
        return false;
    }

    void print()
    {
        Node<S>* temp = top;
        while (temp != nullptr)
        {
            cout << temp->data << "\t";

            if (temp->next == nullptr)
            {
                break;
            }

            temp = temp->next;
        }
    }

    ~Stack()
    {
        top = nullptr;
    }
};

class XMLData
{
public:
    int lineNumber;
    bool StartOrEnd;
    //0 for start 1 for end
    string tagText;

    XMLData()
    {
        lineNumber = 0;
        StartOrEnd = 0;
    }

    bool CheckQuotes(string& obj)
    {
        string temp = obj;
        size_t n = std::count(temp.begin(), temp.end(), '"');
        if (n % 2 == 0)
        {
            return true;
        }
    }

    bool checkcomment(string& obj)
    {
        int check1 = obj.find("<!");
        size_t n = std::count(obj.begin(), obj.end(), '-');
        if (check1 != -1)
        {
            if (n == 4)
            {
                return true;
            }
        }
        return true;
    }

    int checktag(string& obj)
    {
        int check = 0;
        int check2 = -1;
        int check1 = obj.find("<");
        int check11 = obj.find("<?");

        if (check1 != -1)
        {
            check2 = obj.find(">");
            if (check2 != -1)
            {
                for (int i = check1 + 1; i < check2; i++)
                {
                    if (obj[i] >= 'A' && obj[i] <= 'Z')
                    {
                        check = 1;
                    }
                }
            }

        }

        // now check for ending tag
        string temp = obj.substr(check2 + 1, obj.length());
        check1 = temp.find("</");
        if (check1 != -1)
        {
            check2 = temp.find(">");
            if (check2 != -1)
            {
                for (int i = check1 + 2; i < check2; i++)
                {
                    if (temp[i] >= 'A' && temp[i] <= 'Z')
                    {
                        if (check == 0)
                            check = 2;

                        return check;
                    }
                }
            }
            else
            {
                if (check == 0)
                    check = 2;

                return check;
            }
        }

        return check;
    }

    bool checkattribute(string& obj)
    {
        int f = obj.find('"');
        if (f != -1)
            if (obj[f - 1] != '=')
                return false;
            else
                return CheckQuotes(obj);
    }

    void PrintXML()
    {
        cout << "TagName = " << tagText << ",  Line_Number = " << lineNumber << ", ";
        if (!StartOrEnd)
        {
            cout << "Starting tag" << endl;
        }
        else
        {
            cout << "Ending tag" << endl;
        }
    }
};

void AnalyzeXML(string filename)
{
    Stack<XMLData> XMLstack;
    bool error = false;
    int lineCounter = 1;
    bool foundError = false;
    bool attributeError = false;
    ifstream fin;
    fin.open(filename);

    while (!fin.eof())
    {
        XMLData xml;
        string line;

        getline(fin, line, '\n');
        //reads whole line so that line number can be tracked

        int return_value = xml.checktag(line);
        if (return_value != 0)
        {
            if (return_value == 1)
                cout << "Error open tag is written in upercase  :" << lineCounter << endl;
            else if (return_value == 2)
                cout << "Error close tag is written in upercase  :" << lineCounter << endl;
        }
        int CurrentLineLength = line.length();

        while (CurrentLineLength > 0)
        {
            int OpeningAngularBracketIndex = line.find('<');
            int ClosingAngularBracketIndex = line.find('>');

            //XML prolog is optional. If it exists, it must come first in the document.

            if (line[OpeningAngularBracketIndex + 1] == '?')
            {
                //if true, it's a prolog
                if (lineCounter == 1)
                {
                    line = line.substr(OpeningAngularBracketIndex + 2, line.size());
                    //ignore s[0]='<' and s[1]='?'
                    CurrentLineLength = line.length();

                    int endOfProlog = line.find("?>");
                    if (endOfProlog == -1)
                        //if false, it means prolog is complete
                    {
                        cout << "ERROR\tThe prolog is not complete.\n";
                        foundError = true;
                        break;
                    }
                }

            }

            //check if it's start of a simple tag with or without attribute.
            else if ((line[OpeningAngularBracketIndex] == '<') && ((line[OpeningAngularBracketIndex + 1] >= 65 && line[OpeningAngularBracketIndex + 1] <= 90) || (line[OpeningAngularBracketIndex + 1] >= 97 && line[OpeningAngularBracketIndex + 1] <= 122)) && (OpeningAngularBracketIndex < ClosingAngularBracketIndex))
            {
                xml.lineNumber = lineCounter;
                xml.StartOrEnd = 0;

                string Tag;
                int FirstSpaceAfterTag = line.find(' ');

                //check if there's an attribute
                if (FirstSpaceAfterTag != -1 && (FirstSpaceAfterTag < ClosingAngularBracketIndex || ClosingAngularBracketIndex == -1))
                {
                    Tag = line.substr(1, FirstSpaceAfterTag - 1);

                    line = line.substr(FirstSpaceAfterTag + 1, line.size());//delete tag name from current line

                    int firstQuoteS = line.find("'");//find first single quote
                    int firstQuoteD = line.find('"');//find first double quote

                    //if first "double quote" exists and there's a charachter after that double quote (i.e., name of attribute)
                    if (firstQuoteD < firstQuoteS && firstQuoteD != -1 && ((line[firstQuoteD + 1] >= 65 && line[firstQuoteD + 1] <= 90) || (line[firstQuoteD + 1] >= 97 && line[firstQuoteD + 1] <= 122)))
                    {
                        line = line.substr(firstQuoteD + 1, line.size());//delete line from start upto first double quote(icluding)

                        int secondQuoteD = line.find('"');//find second double quote
                        ClosingAngularBracketIndex = line.find('>');//update index of '>'

                    }
                }

            }

            //icheck if it's end of a simple tag with or without attribute.
            else if ((line[OpeningAngularBracketIndex] == '<') && (line[OpeningAngularBracketIndex + 1] == '/') && ((line[OpeningAngularBracketIndex + 2] >= 65 && line[OpeningAngularBracketIndex + 2] <= 90) || (line[OpeningAngularBracketIndex + 2] >= 97 && line[OpeningAngularBracketIndex + 2] <= 122)))
            {
                line = line.substr(OpeningAngularBracketIndex + 2, line.size());
                int TagLength = line.find('>');
                string Tag = line.substr(0, TagLength);

                int x = xml.tagText.compare(Tag);
                if (x == 0)
                {
                    XMLstack.pop(xml);
                    //cout<<"Popped: "; xml.PrintXML();
                }

                line = line.substr(TagLength + 1, line.size());
                CurrentLineLength = line.length();
            }

            //check if there are double dashes in the comment
            else if (line.find("--") != line.find("-->"))
            {
                if (OpeningAngularBracketIndex == -1 && ClosingAngularBracketIndex == -1)
                {
                    CurrentLineLength = 0;
                }
                else
                {
                    cout << "Error!!!\tAt line " << lineCounter << ", Two dashes in the middle of a comment are not allowed.";
                    foundError = true;
                    break;
                }

            }
        }

        lineCounter++;
    }

    //show error if stack is not empty
    if (!XMLstack.IsEmpty() && !attributeError && !foundError)
    {
        cout << "---- ERROR ---- \tFound ";

        if (XMLstack.Top().StartOrEnd == 0)
            cout << "Starting";
        else
            cout << "Ending";

        cout << " tag \"" << XMLstack.Top().tagText << "\" at line " << XMLstack.Top().lineNumber << " but it has no ";

        if (XMLstack.Top().StartOrEnd == 0)
            cout << "Ending";
    }

    else if (!foundError && !attributeError)
        if (!error)
            cout << "No Error found in this xml file.\n";

    fin.close();
}

int main()
{
    AnalyzeXML("xml.txt");

    system("pause");
    return 0;
}