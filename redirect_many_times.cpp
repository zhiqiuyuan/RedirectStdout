#include <iostream>

#include <fstream>

#include <stack>

 

#include <unistd.h>

#include <fcntl.h>

 

using namespace std;

 

template <typename T>

void print_stk(stack<T> arr)

{

    cout << "[sz:" << arr.size() << "] ";

    while (arr.empty() == 0)

    {

        cout << arr.top() << " ";

        arr.pop();

    }

    cout << endl;

}

 

// if <des> is descriptor to <file>, then we call <des>'s refer is <file>

// construct: redirect descriptor 1's refer ori_file to file(constructor's param)(and push ori_file to static stk top)

// destruct: restore descriptor 1's refer to stk top(and pop stk)

// static stk design ensures when there's no RedirectStdout instance, descriptor 1's refer is stdout

class RedirectStdout

{

public:

    static stack<int> ori_file_stk;

 

    /* redirect descriptor 1's refer ori_file to file(and push ori_file to static stk top) */

    // append: if set, then open file with |O_APPEND

    RedirectStdout(const char *file, int append = 0)

    {

        // TODO: check these syscall's return value

        // now 1 is descriptor to ori_file(if stk is empty, ori_file is STDOUT; else ori_file is stk.top())

 

        int pfd;

        if (append)

        {

            pfd = open(file, O_WRONLY | O_CREAT | O_APPEND, 0777);

        }

        else

        {

            pfd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0777);

        }

        // now pfd is descriptor to file

        int saved = dup(1); // now 1 and saved both are descriptor to ori_file(1 previously refer to)

        // int dup2(int oldfd, int newfd);

        dup2(pfd, 1); // would close descriptor 1 first, then now 1 and pfd both are descriptor to file(pfd previously refer to)

        close(pfd);   // close descriptor pfd

 

        ori_file_stk.push(saved);

        // now saved(stk.top()) is descriptor to ori_file, 1 is descriptor to file

    }

    /* fflush and restore descriptor 1's refer to stk top(and pop stk) */

    ~RedirectStdout()

    {

        // TODO: check these syscall's return value

        fflush(stdout); // NOTE that flush the buffer is necessary to indeed push content into file

        int saved = ori_file_stk.top();

        ori_file_stk.pop();

        dup2(saved, 1); // would close descriptor 1 first, then now 1 and saved both are descriptor to "saved previously refer to"

        close(saved);   // close descriptor saved

 

        // now only 1 is descriptor to "saved previously refer to"

    }

};

 

stack<int> RedirectStdout::ori_file_stk;

 

int main()

{

    print_stk<int>(RedirectStdout::ori_file_stk); // print in console: {}

    // now 1 refer to STDOUT

    {

        RedirectStdout ins1("ins1");

        // now 1 refer to "ins1"

        print_stk<int>(RedirectStdout::ori_file_stk); // print in ins1: {des_to_STDOUT}

 

        {

            RedirectStdout ins2("ins2");

            // now 1 refer to "ins2"

            print_stk<int>(RedirectStdout::ori_file_stk); // print in ins2: {des_to_ins1,des_to_STDOUT}

        }

        // ins2 destruct: now 1 refer to "ins1"

        print_stk<int>(RedirectStdout::ori_file_stk); // print in ins1: {des_to_STDOUT}

    }

    // ins1 destruct: now 1 refer to STDOUT

    print_stk<int>(RedirectStdout::ori_file_stk); // print in console: {}

 

    return 0;

}
