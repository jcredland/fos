/*
 * Utility to take a keyboard .script file and generate any necessary 
 * C++ arrays. 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>

using namespace std; 

struct KeyData
{
    string key_code; 
    char32_t normal; 
    char32_t shifted;
};

vector<KeyData> key_mappings;

void usage()
{
    printf("usage: keyboard_data_gen <input_script> <output_header_file>\n"); 
    printf("Keyboard script files have // double slashes as comments.\n"); 
}

vector<string> get_split_strings(const string & source)
{
    string new_string; 
    vector<string> result;

    auto a = source.begin(); 

    while (a != source.end())
    {
        if (! isspace(*a))
            new_string += *a;
        else
            if (new_string.size() > 0)
            {
                result.push_back(new_string); 
                new_string.clear(); 
            }

        a++;
    }

    if (new_string.size() > 0)
        result.push_back(new_string); 

    return result; 
}


bool is_positive_integer(const string & s)
{
    return ! s.empty() && find_if(s.begin(), s.end(), [](char c) 
            { 
                return ! std::isdigit(c); 
            }) == s.end();
}

char32_t get_unicode_for_utf8(const string & s)
{
    if ((s[0] & 0x80) == 0)
        return s[0];

    /* insert rest of utf8 decoding here. */
    return 0; 
}
/** 
 * If the string has mutiple characters then treat it as the character
 * number.  Otherwise just take the character number. 
 */
char32_t get_unicode_character_from_string(const string & s)
{
    if ((s.size() > 1) && is_positive_integer(s))
        return (char32_t) stoul(s); 
    else
        return get_unicode_for_utf8(s);
}

KeyData get_key_data(char * buf)
{
    vector<string> fields = get_split_strings(buf);

    if (fields.size() == 0)
    {
        cout << "error: invalid line" << endl << buf << endl;
        exit(1); 
    }

    KeyData d; 
    d.key_code = fields[0];

    if (fields.size() == 1)
    {
        d.normal = 0; 
        d.shifted = 0;
    }

    if (fields.size() == 2)
    {
        d.normal = get_unicode_character_from_string (fields[1]);
        d.shifted = d.normal;
    }

    if (fields.size() == 3)
    {
        d.normal = get_unicode_character_from_string (fields[1]);
        d.shifted = get_unicode_character_from_string (fields[2]);
    }

    return d;
}

void write_output_arrays(char * output_filename)
{
    ofstream of {output_filename};
    const char * key_mapping_name = "keymap_uk"; 

    of << "/* Keyboard mapping data.  Created by keyboard_data_gen from a script file. */" << endl;
    of << "KeyAttributes " << key_mapping_name << "[] = {" << endl;
    bool first_time = true;

    for (auto m: key_mappings)
    {
        if (! first_time)
            of << "," << endl;

        of << "    { " << m.key_code << ", " << m.normal << ", " << m.shifted << " }"; 

        first_time = false; 
    }

    of << endl << "};" << endl;
}

int main(int argc, char ** argv)
{
    if (argc != 3) 
    {
        usage(); 
        exit(1); 
    }

    FILE * f = fopen(argv[1], "r");
    const int count = 200; 
    char buf[count];

    while (fgets(buf, count, f) != 0)
    {
        /* Terminate the string at the location of a comment. */
        char * comment_position = strstr(buf, "//"); 
        
        if (comment_position) 
            *comment_position = 0;

        /* Ignore blank lines. */
        char *end_of_whitepace = buf; 

        while (isspace(*end_of_whitepace))
            end_of_whitepace++;

        if (*end_of_whitepace == '\0')
            continue;


        /* And add any remaining data to the file. */
        key_mappings.push_back(get_key_data(buf)); 
    }

    fclose(f); 

    write_output_arrays(argv[2]); 
    
    return 0;
}   

