#include <iostream>
#include <cstdio> // fopen, fclose, fread, fwrite, BUFSIZ
#include <ctime>
using namespace std;

int main()
{
    clock_t start, end;
    start = clock();

    // BUFSIZE default is 8192 bytes
    // BUFSIZE of 1 means one chareter at time
    // good values should fit to blocksize, like 1024 or 4096
    // higher values reduce number of system calls
    // size_t BUFFER_SIZE = 4096;

    char buf[BUFSIZ];
    size_t size;

    FILE *source = fopen("100MB.bin", "rb");
    FILE *dest = fopen("Folder_1/to.bin", "wb");

    // clean and more secure
    // feof(FILE* stream) returns non-zero if the end of file indicator for stream is set

    while (size = fread(buf, 1, BUFSIZ, source))
    {
        fwrite(buf, 1, size, dest);
    }

    fclose(source);
    fclose(dest);

    end = clock();

    cout << "CLOCKS_PER_SEC " << CLOCKS_PER_SEC << "\n";
    cout << "CPU-TIME START " << start << "\n";
    cout << "CPU-TIME END " << end << "\n";
    cout << "CPU-TIME END - START " << end - start << "\n";
    cout << "TIME(SEC) " << static_cast<double>(end - start) / CLOCKS_PER_SEC << "\n";

    return 0;
}