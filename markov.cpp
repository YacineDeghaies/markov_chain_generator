#include <ctime>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>


class markov {
public:
    void create(const std::string& file, unsigned int keyLen, unsigned int words ) {
               
        // open the file . no need to put std::ios_base::in
        std::ifstream f( file.c_str(), std::ios_base::in );
        //before using a newly opened file check if file opened correctly
        //first check: if file is in failed state
        if(f.fail()){
            std::cerr << "Error openeing file: "<<file<<std::endl;
            return; // or use a loop to ask for a different file name
        }

        //copy content of file inside a string
        fileBuffer = std::string( ( std::istreambuf_iterator<char>( f ) ), std::istreambuf_iterator<char>() ); 

        //close the file
        f.close(); //

        // if there is not content return
        if( fileBuffer.length() < 1 ) return;
        
        // 
        createDictionary( keyLen );
        createText( words - keyLen );
    }
private:
    //Generates a new piece of text based on the dictionary.
    void createText( int w ) {
        std::string key, first, second;
        size_t next;
        std::map<std::string, std::vector<std::string> >::iterator it = dictionary.begin();
        // advance the dict iterator n-random number of times
        // rand() % dictionary.size() will make sure to give us a random number that will not surpass the size of the vector
        std::advance( it, rand() % dictionary.size() );

        //take the key
        key = ( *it ).first;
        //print the key
        std::cout << key;

        while( true ) { // take a random value from the vector of words we have for our specific key
            std::vector<std::string> d = dictionary[key];
            if( d.size() < 1 ) break;
            // in case we have only 1 element inside the vector
            // suppose: rand() == 1608 and d.size() ==1 => we will get d[0]
            second = d[rand() % d.size()];
            if( second.length() < 1 ) break;
            std::cout << " " << second;
            // w is the word count we need to print for our new generated text
            if( --w < 0 ) break;
            // get the position of the second word inside key
            next = key.find_first_of( 32, 0 );
            //extract the word
            first = key.substr( next + 1 );
            // make a new key based on previous output ( THIS IS WHERE THE MARKOV CHAIN PART COMES UP)
            // Markov chain : the next key is gonna be determined only by 1 output which is the variable second
            key = first + " " + second;
        }
        std::cout << "\n";
    }

    // create a dictionary for the whole content of the file
    void createDictionary( unsigned int kl ) {
        std::string w1, key; // two strings 
        size_t wc = 0, pos, next; // 
        
        // Look for the first character in the CONTENT 
        // 32 means a space in ASCII
        // 0 is where the search should start, it's in the range [0, fileBuffer.size() )
        next = fileBuffer.find_first_not_of( 32, 0 ); 
        
        // if no character was found, return.
        if( next == std::string::npos ) return;
        while( wc < kl ) { // in this part we have to find the key
            // pos : find the first space character
            pos = fileBuffer.find_first_of( ' ', next );
            
            // substr(pos, length)
            // next has the position of the first found character
            // pos has the position of the first found space character
            // pos-next gives us back the size of the word we found

            // extract the first word you find
            w1 = fileBuffer.substr( next, pos - next );

            // "FirstWord "
            key += w1 + " ";

            //find the next character starting from the poition right after the space chatacter
            next = fileBuffer.find_first_not_of( 32, pos + 1 );
            if( next == std::string::npos ) return;
            // increment word counter until we reach the keyLength(number of words to be taken)
            wc++;
        }// Example: 
        // KeyLength:2 key:"Alice was "

        //remove the trailing space character
        //key is the same here
        key = key.substr( 0, key.size() - 1 ); // key:"Alice was"

        while( true ) { // find the word that comes right after our key
            //find the next non-space character
            next = fileBuffer.find_first_not_of( 32, pos + 1 );

            if( next == std::string::npos ) return;
            
            // find the next space 
            pos = fileBuffer.find_first_of( 32, next );
            
            //extract the word
            w1 = fileBuffer.substr( next, pos - next );

            if( w1.size() < 1 ) break;
            // if we don't have the word added into our vector we don't need add it again
            //find will return iterator to the first found element or end iterator if no such element is found.
            if( std::find( dictionary[key].begin(), dictionary[key].end(), w1 ) == dictionary[key].end() ) 
                dictionary[key].push_back( w1 ); // if element is already there no need to add it again

            // key: key:"Alice was"
            // new key: "was beginning"
            // substr from the char right after the space to the last char in the key + the new found work = new_key
            key = key.substr( key.find_first_of( 32 ) + 1 ) + " " + w1;
        }
    }

    std::string fileBuffer;
    std::map<std::string, std::vector<std::string> > dictionary;
};
int main( int argc, char* argv[] ) {

    srand( unsigned( time( 0 ) ) );
    markov m;
    std::string fileName;
    std::cin >> fileName;  // alice.txt

    m.create( fileName,3, 200 );

    return 0;
}
