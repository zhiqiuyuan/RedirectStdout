#include <iostream>
#include <unistd.h>
#include <fcntl.h>
using namespace std;

// implement redirect stdout in class: exception safety
// destructors of all objects are ensured to be called when exception occurs(ensured by compiler)
// redirect stdout to file when constructing instance, restore when destructing
// NOTE: only one success instance is allowed
class RedirectStdout
{
    int saved;
    int redirected_success;
    static int success_instance_count;

public:
    int is_success()
    {
        return redirected_success;
    }
    // redirect stdout to file
    RedirectStdout(const char *file)
    {
        if (success_instance_count == 1)
        {
            cout << "only one success instance is allowed!" << endl;
            redirected_success = 0;
            return;
        }
        // TODO: check these syscall's return value
        // now 1 is descriptor to STDOUT
        int pfd = open(file, O_WRONLY | O_CREAT, 0777); // now pfd is descriptor to "bin/.gconsole_tmp_out"
        saved = dup(1);                                 // now 1 and saved both are descriptor to STDOUT
        // int dup2(int oldfd, int newfd);
        dup2(pfd, 1); // would close descriptor 1 first, then now 1 and pfd both are descriptor to "bin/.gconsole_tmp_out"(pfd previously pointed to)
        close(pfd);   // close descriptor pfd
        redirected_success = 1;
        ++success_instance_count;
    }
    // fflush and restore stdout
    ~RedirectStdout()
    {
        if (redirected_success)
        {
            --success_instance_count;
            // TODO: check these syscall's return value
            fflush(stdout); // NOTE that flush the buffer is necessary to indeed push content into file
            dup2(saved, 1); // would close descriptor 1 first, then now 1 and saved both are descriptor to STDOUT(saved previously pointed to)
            close(saved);   // close descriptor saved}
        }
    }
};

int RedirectStdout::success_instance_count = 0;
int main()
{
    {
        RedirectStdout ins1("1");              // success, stdout is redirected to file"1"
        cout << ins1.is_success() << endl;     // success, print to file"1"
        cout << "This goes into file" << endl; // print to file"1"
        RedirectStdout ins2("2");              // fail, fail msg print to file"1"
        cout << ins2.is_success() << endl;     // print to file"1"
        RedirectStdout ins3("3");              // fail, fail msg print to file"1"
        cout << ins3.is_success() << endl;     // print to file"1"
    }
    // destruct: restore stdout
    cout << "This goes into console" << endl;

    return 0;
}
