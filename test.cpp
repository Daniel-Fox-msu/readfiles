#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdint>
#include <vector>
using std::vector;
#include <cstdio>
#include <cerrno>
#include <windows.h>
#include <tchar.h>
#include <fcntl.h>
#include <io.h>


/*
Given a file name as a char array, add each byte from the file
to a string and return that string.
*/
std::string get_file_contents(const char *filename)
{
  std::FILE *fp = std::fopen(filename, "rb");
  if (fp)
  {
    std::string contents;
    std::fseek(fp, 0, SEEK_END);
    contents.resize(std::ftell(fp));
    std::rewind(fp);
    std::fread(&contents[0], 1, contents.size(), fp);
    std::fclose(fp);
    return(contents);
  }
  throw(errno);
}

//convert 4 bytes into a 32 bit int (little endian)
int bytes_to_int32(std::string bytes){
	int result = int((unsigned char)(bytes[0]) |
					(unsigned char)(bytes[1]) << 8 |
					(unsigned char)(bytes[2]) << 16 |
					(unsigned char)(bytes[3]) << 24);
	return result;
}

//convert 2 bytes into a 16 bit int (little endian)
int bytes_to_int16(std::string bytes){
	int result = int((unsigned char)(bytes[0]) |
					(unsigned char)(bytes[1]) << 8 );
	return result;
}

//convert 2 bytes to a gb2312 chinese character
std::wstring bytes_to_gb2312(const std::string &str)
{
    int num_chars = MultiByteToWideChar(936, 0, str.c_str(), str.length(), NULL, 0);
    std::wstring wstrTo;
    if (num_chars)
    {
        wstrTo.resize(num_chars);
        if (MultiByteToWideChar(936, 0, str.c_str(), str.length(), &wstrTo[0], num_chars))
        {
            return wstrTo;
        }
    }
}



int main(){
	_setmode(_fileno(stdout), _O_U16TEXT); //set output to display Unicode instead of ascii
	
    char fname[] = "1001-c.gnt";
	std::string contents = get_file_contents(fname);
	
	//first 4 bytes of the file are the total bytes for that entry, as a 32 bit int
	std::string bytes_sz {contents[0], contents[1], contents[2], contents[3]};
	int entry_size = bytes_to_int32(bytes_sz);
	
	//next 2 bytes are the chinese character, encoded in gb2312
	std::string bytes_ch {contents[4], contents[5]};
	//convert bytes to unicode character
	std::wstring gb_char = bytes_to_gb2312(bytes_ch);
	
	
	//next 4 bytes are the width and height of the bitmap, as two 16 bit ints
	std::string bytes_width {contents[6], contents[7]};
	std::string bytes_height {contents[8], contents[9]};
	
	int width = bytes_to_int16(bytes_width);
	int height = bytes_to_int16(bytes_height);
	
	
	

	std::wcout << "Total bytes for this entry: " << entry_size << std::endl;
	std::wcout << "Bitmap width of this entry: " << width << std::endl;
	std::wcout << "Bitmap height of this entry: " << height << std::endl;
	std::wcout << "Correct character for this entry: " << gb_char << std::endl;
	
	
	
}
