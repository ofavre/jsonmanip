/*
 * main.cpp
 *
 *  Created on: 11 août 2011
 *      Author: ofavre
 */

#include "common.hpp"

bool verbose = false;
bool quiet = false;
bool pretty = false;
bool multiple = false;
string indentation = "  ";
vector<Path> paths;
StyledStreamWriter styledWriter (indentation);
FastWriter fastWriter;

void processInput(istream& in);

int main(int argc, char** argv)
{
    paths.push_back(Path("."));

    bool noFileGiven = true;
    int arg;
    for (arg = 1; arg < argc; arg++)
    {
        if (!strcmp(argv[arg], "--help") || !strcmp(argv[arg], "-h"))
        {
            cout << "Usage: " << argv[0] << " -h | --help" << endl;
            cout << "Usage: " << argv[0] << " {[-v|-q|-n] [-s|-l] [-m|-p [-i C]] [-e P] [file]} [...]" << endl;
            cout << "Reads json, can extract and [un]pretty print." << endl;
            cout << "Each argument is processed immediately. So you can change the behavior between files." << endl;
            cout << "    -v, --verbose             Be verbose." << endl;
            cout << "    -q, --quiet               Do not print any undesired output as errors or warnings." << endl;
            cout << "    -n, --normal              Normal output verbosity. (Default)" << endl;
            cout << "    -s, --single              Input is a single, possibly multiline, document. (Default)" << endl;
            cout << "    -l, --multiple            Input consists of one document per line." << endl;
            cout << "  PRINT_OPTIONS:" << endl;
            cout << "    -m, --minify              Minifies the output. (Default)" << endl;
            cout << "    -p, --pretty              Pretty prints the output." << endl;
            cout << "    -i CHAR, --indent CHAR    Specify the indentation character. \\t and co. are parsed. (Default: two spaces)" << endl;
            cout << "  EXTRACT_OPTIONS:" << endl;
            cout << "    -e PATH, --path PATH      Extracts a value to print." << endl;
            cout << "                              . is the root object (optional). (Default value)" << endl;
            cout << "                              .key gets the named key." << endl;
            cout << "                              .[0] gets the first value of an array." << endl;
            cout << "                              .key[0] gets the first value of the array named key." << endl;
            cout << "                              You can chain path." << endl;
            cout << "    +e PATH, --+path PATH     Adds another path to extract." << endl;
            cout << "                              The previous option resets the path to be extracted." << endl;
            exit(0);
        }
        else if (!strcmp(argv[arg], "--verbose") || !strcmp(argv[arg], "-v"))
        {
            verbose = true;
            quiet = false;
        }
        else if (!strcmp(argv[arg], "--quiet") || !strcmp(argv[arg], "-v"))
        {
            quiet = true;
            verbose = false;
        }
        else if (!strcmp(argv[arg], "--normal") || !strcmp(argv[arg], "-n"))
        {
            verbose = false;
            quiet = false;
        }
        else if (!strcmp(argv[arg], "--multiple") || !strcmp(argv[arg], "-l"))
        {
            multiple = true;
        }
        else if (!strcmp(argv[arg], "--single") || !strcmp(argv[arg], "-s"))
        {
            multiple = false;
        }
        else if (!strcmp(argv[arg], "--path") || !strcmp(argv[arg], "-e") || !strcmp(argv[arg], "--+path") || !strcmp(argv[arg], "+e"))
        {
            if (!strcmp(argv[arg], "--path") || !strcmp(argv[arg], "-e"))
                paths.clear();
            if (arg + 1 < argc)
                paths.push_back(Path(argv[++arg]));
            else if (!quiet)
                cerr << "Parameter " << argv[arg] << " takes an argument" << endl;
        }
        else if (!strcmp(argv[arg], "--pretty") || !strcmp(argv[arg], "-p"))
        {
            pretty = true;
        }
        else if (!strcmp(argv[arg], "--indent") || !strcmp(argv[arg], "-i"))
        {
            if (arg + 1 < argc)
            {
                indentation.assign(argv[++arg]);
                if (indentation.at(0) == '\\')
                {
                    switch (indentation.at(1))
                    {
                        case '0':
                            indentation = "\0";
                            break;
                        case 't':
                            indentation = "\t";
                            break;
                        case 'n':
                            indentation = "\n";
                            break;
                        case 'r':
                            indentation = '\r';
                            break;
                        case '\\':
                            indentation = '\\';
                            break;
                        default:
                            if (!quiet)
                                cerr << "Unknown escaped character. Using \"" << indentation << "\" as is!" << endl;
                            break;
                    }
                }
            }
            else
                if (!quiet)
                    cerr << "Parameter " << argv[arg] << " takes an argument" << endl;
            styledWriter = StyledStreamWriter (indentation);
        }
        else if (!strcmp(argv[arg], "--minify") || !strcmp(argv[arg], "-m"))
        {
            pretty = false;
        }
        else
        {
            noFileGiven = false;

            string file = argv[arg];
            ifstream fin;

            if (file != "-")
            {
                if (verbose)
                    cerr << "Reading file \"" << file << "\"" << endl;
                fin.open(file.c_str());
            }
            else if (verbose)
                    cerr << "Reading standard input" << endl;

            processInput(fin.is_open() ? fin : cin);

            if (file == "-")
            {
                // Clear unread input
                cin.sync();
                // Be ready for more!
                cin.clear();
            }

            if (fin.is_open())
                fin.close();
        }
    }

    if (noFileGiven)
    {
        if (verbose)
                cerr << "Reading standard input" << endl;

        processInput(cin);
    }

    return EXIT_SUCCESS;
}

void processInput(istream& in)
{
    long l = 0;
    string line;
    Value root;
    Reader reader;

    string query;
    while (!in.eof())
    {
        l++;

        if (!multiple)
            getline(in, line, (char)EOF);
        else
            getline(in, line);
        if (line.empty())
            continue;

        if (!reader.parse(line, root, false))
        {
            if (!quiet)
            {
                cerr << reader.getFormattedErrorMessages();
                if (multiple)
                    cerr << "Error at line " << l << " within the current input file."<< endl;
            }
            continue;
        }

        for (vector<Path>::const_iterator it = paths.begin() ; it < paths.end() ; it++)
        {
            Value value = it->resolve(root);

            if (pretty)
                styledWriter.write(cout, value);
            else
                cout << fastWriter.write(value);
        }

    }
}
