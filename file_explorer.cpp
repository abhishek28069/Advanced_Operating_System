#include <iostream>
#include <bits/stdc++.h>
#include <iomanip>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <ctype.h>
#include <cstring>
#include <ctime>
#include <unistd.h>
#include <termios.h>
#include <signal.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <pwd.h>
#include <grp.h>

using namespace std;

struct files
{
    string name;
    string type;
    string dir_path;
    string permissions;
    string user_name;
    string group_name;
    string date;
    string size;
};
vector<struct files> files_list;
string current_dir = get_current_dir_name();
string parent_dir, home_dir;
int terminal_height;
int top = 0, bottom;
deque<string> forward_stack;
deque<string> backward_stack;
struct termios orig_termios;
int cursor = 0;

void get_terminal_height()
{
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    terminal_height = w.ws_row;
}

void disableRawMode()
{
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}
void enableRawMode()
{
    tcgetattr(STDIN_FILENO, &orig_termios);
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON);
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) != 0)
    {
        cout << "Unable to Switch to Normal Mode";
        return;
    }
}

bool comparator(struct files a, struct files b)
{
    if (a.permissions[0] == 'd' && b.permissions[0] == 'd')
    {
        return a.name < b.name;
    }
    else if (a.permissions[0] == 'd' && b.permissions[0] == '-')
    {
        return true;
    }
    else if (a.permissions[0] == '-' && b.permissions[0] == 'd')
    {
        return false;
    }
    else
    {
        return a.name < b.name;
    }
}

void clrscr(void)
{
    cout << "\033[2J\033[1;1H";
}

string get_absolute_path(string path)
{
    string absolute_path;
    if (path[0] == '/')
    {
        // absolute
        absolute_path = path;
    }

    else if (path[0] == '.' && path[1] == '.')
    {
        // relative
        absolute_path = parent_dir + path.substr(2);
    }
    else if (path[0] == '.')
    {
        // relative
        absolute_path = current_dir + path.substr(1);
    }
    else if (path[0] == '~')
    {
        // relative from home
        absolute_path = home_dir + path.substr(1);
    }
    else
    {
        absolute_path = current_dir + "/" + path;
    }
    return absolute_path;
}

string print_permissions(struct stat file)
{
    string permissions = "";
    if (S_ISDIR(file.st_mode))
        permissions += "d";
    else
        permissions += "-";
    if (S_IRUSR & file.st_mode)
        permissions += "r";
    else
        permissions += "-";
    if (S_IWUSR & file.st_mode)
        permissions += "w";
    else
        permissions += "-";
    if (S_IXUSR & file.st_mode)
        permissions += "x";
    else
        permissions += "-";
    if (S_IRGRP & file.st_mode)
        permissions += "r";
    else
        permissions += "-";
    if (S_IWGRP & file.st_mode)
        permissions += "w";
    else
        permissions += "-";
    if (S_IXGRP & file.st_mode)
        permissions += "x";
    else
        permissions += "-";
    if (S_IROTH & file.st_mode)
        permissions += "r";
    else
        permissions += "-";
    if (S_IWOTH & file.st_mode)
        permissions += "w";
    else
        permissions += "-";
    if (S_IXOTH & file.st_mode)
        permissions += "x";
    else
        permissions += "-";

    return permissions;
}

// void populate_files_list()
// {
//     const char *cwd = get_current_dir_name();
//     current_dir = cwd;
//     parent_dir = current_dir.substr(0, current_dir.find_last_of('/'));
//     struct stat t;
//     struct dirent *entry;
//     DIR *dir = opendir(cwd);
//     files_list.clear();
//     if (dir != NULL)
//     {
//         while ((entry = readdir(dir)) != NULL)
//         {
//             struct files file;
//             string file_name = entry->d_name;
//             string path_string = string(cwd) + "/" + file_name;
//             char *path = new char[path_string.length() + 1];
//             strcpy(path, path_string.c_str());
//             stat(path, &t);
//             char *date = new char[20];
//             strftime(date, 20, "%a %d/%m/%y %R", localtime(&(t.st_ctime)));
//             string permissions = print_permissions(t);
//             passwd *user_name = getpwuid(t.st_uid);
//             group *group_name = getgrgid(t.st_gid);
//             string type;
//             string dir_path;
//             if (S_ISDIR(t.st_mode))
//             {
//                 type = "dir";
//                 dir_path = path_string;
//             }
//             else
//             {
//                 type = "file";
//                 dir_path = "";
//             }
//             files_list.push_back(files());
//             files_list[files_list.size() - 1].name = file_name;
//             files_list[files_list.size() - 1].type = type;
//             files_list[files_list.size() - 1].dir_path = dir_path;
//             files_list[files_list.size() - 1].user_name = user_name->pw_name;
//             files_list[files_list.size() - 1].group_name = group_name->gr_name;
//             files_list[files_list.size() - 1].date = date;
//             files_list[files_list.size() - 1].size = to_string(t.st_size) + " Bytes";
//             files_list[files_list.size() - 1].permissions = permissions;
//         }
//         sort(files_list.begin(), files_list.end(), comparator);
//         closedir(dir);
//     }
// }

void populate_files_list(const char *path = current_dir.c_str())
{
    const char *cwd = path;
    current_dir = path;
    parent_dir = current_dir.substr(0, current_dir.find_last_of('/'));
    struct stat t;
    struct dirent *entry;
    DIR *dir = opendir(cwd);
    files_list.clear();
    if (dir != NULL)
    {
        while ((entry = readdir(dir)) != NULL)
        {
            struct files file;
            string file_name = entry->d_name;
            string path_string = string(cwd) + "/" + file_name;
            char *path = new char[path_string.length() + 1];
            strcpy(path, path_string.c_str());
            stat(path, &t);
            char *date = new char[20];
            strftime(date, 20, "%a %d/%m/%y %R", localtime(&(t.st_ctime)));
            string permissions = print_permissions(t);
            passwd *user_name = getpwuid(t.st_uid);
            group *group_name = getgrgid(t.st_gid);
            string type;
            string dir_path;
            if (S_ISDIR(t.st_mode))
            {
                type = "dir";
                dir_path = path_string;
            }
            else
            {
                type = "file";
                dir_path = "";
            }
            files_list.push_back(files());
            files_list[files_list.size() - 1].name = file_name;
            files_list[files_list.size() - 1].type = type;
            files_list[files_list.size() - 1].dir_path = dir_path;
            files_list[files_list.size() - 1].user_name = user_name->pw_name;
            files_list[files_list.size() - 1].group_name = group_name->gr_name;
            files_list[files_list.size() - 1].date = date;
            files_list[files_list.size() - 1].size = to_string(t.st_size) + " Bytes";
            files_list[files_list.size() - 1].permissions = permissions;
        }
        sort(files_list.begin(), files_list.end(), comparator);
        closedir(dir);
    }
}

void print_files_list(string mode = "Normal Mode", string status = "")
{
    // cout << "--------------------------------------------------------------" << endl;
    clrscr();
    get_terminal_height();
    int loop_limit;
    loop_limit = min((int)files_list.size(), terminal_height - 3);
    bottom = top + loop_limit;
    if (cursor == bottom)
    {
        top++;
        bottom++;
    }
    if (cursor >= 0 && cursor == top - 1)
    {
        top--;
        bottom--;
    }
    for (int i = top; i < bottom; i++)
    {
        if (i == cursor)
            cout << "➡️";
        else
            cout << " ";
        cout << setw(15) << files_list[i].permissions;
        cout << setw(20) << files_list[i].user_name;
        cout << setw(20) << files_list[i].group_name;
        cout << setw(20) << files_list[i].size;
        cout << setw(20) << files_list[i].date;
        if (files_list[i].name.length() > 60)
            cout << "\t\t" << files_list[i].name.substr(0, 59) + "...";
        else
            cout << "\t\t" << files_list[i].name;
        cout << endl;
    }
    for (int j = loop_limit; j < terminal_height - 3; j++)
    {
        cout << endl;
    }
    cout << "Current Directory - " << current_dir << endl;
    cout << mode << ": ";
    if (status == "")
        cout << current_dir << endl;
    else
        cout << status << endl;
    // cout << endl
    //      << "Backward - ";
    // for (auto &i : backward_stack)
    // {
    //     cout << i << "  ";
    // }
    // cout << endl;
    // cout << endl
    //      << "Forward - ";
    // for (auto &i : forward_stack)
    // {
    //     cout << i << "  ";
    // }
    // cout << endl;
    // cout << "Current - " << current_dir << endl;
    // cout << "Parent - " << parent_dir << endl;
}

void move_up()
{
    cursor > 0 && cursor--;
    print_files_list();
}

void move_down()
{
    cursor < files_list.size() - 1 && cursor++;
    print_files_list();
}

void go_to_home()
{
    backward_stack.push_back(current_dir);
    struct passwd *pw = getpwuid(getuid());
    char *home = pw->pw_dir;
    home_dir = home;
    cursor = 0;
    populate_files_list(home);
    print_files_list();
}

void go_to_parent()
{
    if (current_dir == "/")
        return;
    if (current_dir == "/home")
        parent_dir += "/";
    forward_stack.clear();
    backward_stack.push_back(current_dir);
    char *path = new char[parent_dir.length() + 1];
    strcpy(path, parent_dir.c_str());
    populate_files_list(path);
    cursor = 0;
    print_files_list();
}

void go_backward()
{
    if (backward_stack.empty())
        return;
    string back = backward_stack.back();
    backward_stack.pop_back();
    forward_stack.push_back(back);
    char *path = new char[back.length() + 1];
    strcpy(path, back.c_str());
    cout << endl
         << "going back to - " << path << endl;
    populate_files_list(path);
    cursor = 0;
    print_files_list();
}

void go_forward()
{
    if (forward_stack.empty())
        return;
    string forwar = forward_stack.back();
    forward_stack.pop_back();
    backward_stack.push_back(forwar);
    char *path = new char[forwar.length() + 1];
    strcpy(path, forwar.c_str());
    populate_files_list(path);
    cout << endl
         << "going front to - " << path << endl;
    cursor = 0;
    print_files_list();
}

void click()
{
    string click_path = files_list[cursor].dir_path;
    // cout << click_path << endl;
    if (files_list[cursor].type == "dir" && files_list[cursor].name != "." && files_list[cursor].name != "..")
    {
        backward_stack.push_back(current_dir);
        char *path = new char[click_path.length() + 1];
        strcpy(path, click_path.c_str());
        populate_files_list(path);
        top = 0;
        cursor = 0;
        print_files_list();
    }
    else if (files_list[cursor].name != "." && files_list[cursor].name != "..")
    {
        pid_t pid = fork();
        if (pid == 0)
        {
            string path_string = string(current_dir) + "/" + files_list[cursor].name;
            char *path = new char[path_string.length() + 1];
            strcpy(path, path_string.c_str());
            execl("/usr/bin/xdg-open", "xdg-open", path, NULL);
            exit(1);
        }
    }
}

void resize(int dummy)
{
    cursor = 0;
    print_files_list();
}

void command_delete_file(string parameters)
{
    // get absolute path
    string delete_file_path = get_absolute_path(parameters);
    // check whether it's a file or directory
    struct stat check1;
    stat(delete_file_path.c_str(), &check1);
    if (S_ISDIR(check1.st_mode))
    {
        cout << "incorrect argument, required file, supplied directory" << endl;
        return;
    }
    int check2 = remove(delete_file_path.c_str());
    if (check2 != 0)
    {
        cout << "delete file error" << endl;
    }
}

void command_delete_dir(string parameters)
{
    string delete_dir_path = get_absolute_path(parameters);
    DIR *directory;
    struct dirent *marker;
    directory = opendir(delete_dir_path.c_str());
    if (directory == NULL)
    {
        cout << "No directory with that name";
    }
    while ((marker = readdir(directory)))
    {
        string dot_check = marker->d_name;
        if (dot_check == "." || dot_check == "..")
        {
            continue;
        }
        string entry_name = delete_dir_path + "/" + dot_check;
        struct stat dir_check;
        stat(entry_name.c_str(), &dir_check);
        if (S_ISDIR(dir_check.st_mode))
        {
            command_delete_dir(entry_name);
        }
        else
        {
            command_delete_file(entry_name);
        }
    }
    closedir(directory);
    remove(delete_dir_path.c_str());
}

bool command_search(string key, string dir_path)
{
    DIR *directory;
    bool is_file = false;
    struct dirent *marker;
    struct stat checker, key_checker;
    stat(get_absolute_path(key).c_str(), &key_checker);
    if (S_ISDIR(key_checker.st_mode))
        is_file = false;
    else
        is_file = true;
    directory = opendir(dir_path.c_str());
    while ((marker = readdir(directory)))
    {
        stat(marker->d_name, &checker);
        if (S_ISDIR(checker.st_mode))
        {
            if (!is_file && key == string(marker->d_name))
            {
                return true;
            }
            if (string(marker->d_name) == "." || string(marker->d_name) == "..")
                continue;
            string next = dir_path + "/" + marker->d_name;
            bool res = command_search(key, next);
            if (res == true)
                return true;
        }
        else
        {
            if (is_file && key == string(marker->d_name))
            {
                return true;
            }
        }
    }
    closedir(directory);
    return false;
}

void copy_file(string entry_path, string destination_path, mode_t modes, uid_t user, gid_t group)
{
    cout << "argument - " << entry_path << endl;
    cout << "created file path - " << destination_path << endl;
    char letter;
    FILE *src = fopen(entry_path.c_str(), "r");
    FILE *dest = fopen(destination_path.c_str(), "w");
    if (dest == NULL)
    {
        printf("Error opening file: %s\n", strerror(errno));
        return;
    }
    // copy contents
    while ((letter = getc(src)) != EOF)
        putc(letter, dest);
    // copy permissions and owner
    chown(destination_path.c_str(), user, group);
    chmod(destination_path.c_str(), modes);
    fclose(src);
    fclose(dest);
}

void copy_dir(string entry_path, string destination_path, mode_t modes, uid_t user, gid_t group)
{
    // cout << "copying dir argument - " << entry_path << endl;
    // cout << "created dest dir path - " << destination_path << endl;
    DIR *directory;
    struct dirent *marker;
    directory = opendir(entry_path.c_str());
    if (directory == NULL)
    {
        cout << "cannot open the directory" << endl;
    }
    while ((marker = readdir(directory)))
    {
        string dot_check = string(marker->d_name);
        if (dot_check == "." || dot_check == "..")
            continue;
        string entry_name = entry_path + "/" + marker->d_name;
        string destination_name = destination_path + "/" + marker->d_name;
        struct stat check;
        stat(entry_name.c_str(), &check);
        if (S_ISDIR(check.st_mode))
        {
            mkdir(destination_name.c_str(), 0777);
            copy_dir(entry_name, destination_name, check.st_mode, check.st_uid, check.st_gid);
        }
        else
        {
            cout << "file - " << dot_check << endl;
            copy_file(entry_name, destination_name, check.st_mode, check.st_uid, check.st_gid);
        }
    }
    closedir(directory);
}

void command_copy(vector<string> parameters)
{
    // remove command
    parameters.erase(parameters.begin());
    // error handling
    if (parameters.size() <= 1)
    {
        print_files_list("Command Mode", "Error, Must need atleast two arguemnts");
        return;
    }
    backward_stack.push_back(current_dir);
    // get destination
    string destination_parameter = parameters.back();
    // remove destination, now vector contains files to be copied
    parameters.pop_back();
    // process destination
    string destination = get_absolute_path(destination_parameter);
    cout << "destination path - " << destination << endl;
    // identify the source whether it's a file or directory
    struct stat t;
    for (int i = 0; i < parameters.size(); i++)
    {
        string entry = parameters[i];
        string entry_path = get_absolute_path(entry);
        cout << "src path - " << entry_path << endl;
        stat(entry_path.c_str(), &t);
        string destination_path = destination + entry_path.substr(entry_path.find_last_of("/"));
        if (S_ISDIR(t.st_mode))
        {
            cout << "detected directory" << endl;
            mkdir(destination_path.c_str(), 0777);
            copy_dir(entry_path, destination_path, t.st_mode, t.st_uid, t.st_gid);
        }
        else
        {
            cout << "detected file" << endl;
            copy_file(entry_path, destination_path, t.st_mode, t.st_uid, t.st_gid);
        }
    }
    populate_files_list(destination.c_str());
    print_files_list("Command Mode", "Succesfully copied! Displaying destination directory now.");
}

void command_move(vector<string> parameters)
{
    // remove command
    parameters.erase(parameters.begin());
    // error handling
    if (parameters.size() <= 1)
    {
        print_files_list("Command Mode", "Error, Must need atleast two arguemnts");
        return;
    }
    backward_stack.push_back(current_dir);
    // get destination
    string destination_parameter = parameters.back();
    // remove destination, now vector contains files to be copied
    parameters.pop_back();
    // process destination
    string destination_path = get_absolute_path(destination_parameter);
    populate_files_list(destination_path.c_str());
    struct stat t;
    for (int i = 0; i < parameters.size(); i++)
    {
        string entry = parameters[i];
        string entry_path = get_absolute_path(entry);
        stat(entry_path.c_str(), &t);
        destination_path += entry_path.substr(entry_path.find_last_of("/"));
        if (S_ISDIR(t.st_mode))
        {
            mkdir(destination_path.c_str(), 0777);
            copy_dir(entry_path, destination_path, t.st_mode, t.st_uid, t.st_gid);
            command_delete_dir(entry_path);
        }
        else
        {
            copy_file(entry_path, destination_path, t.st_mode, t.st_uid, t.st_gid);
            command_delete_file(entry_path);
        }
    }

    print_files_list("Command Mode", "Succesfully moved! Displaying destination directory now.");
}

void command_rename(vector<string> parameters)
{
    // error handling
    if (parameters.size() < 3)
    {
        print_files_list("Command Mode", "Error, Must need atleast two arguemnts");
        return;
    }
    string src_path = get_absolute_path(parameters[1]);
    string dest = src_path.substr(0, src_path.find_last_of("/")) + "/" + parameters[2];
    rename(get_absolute_path(parameters[1]).c_str(), dest.c_str());
    print_files_list("Command Mode", "Renamed " + parameters[1] + " to " + parameters[2]);
}

void command_create_file(vector<string> parameters)
{
    // error handling
    if (parameters.size() != 3)
    {
        print_files_list("Command Mode", "Error, Must need atleast two arguemnts");
        return;
    }
    parameters.erase(parameters.begin());
    string new_file_name = parameters[0];
    string destination_dir_name;
    if (parameters.size() == 1)
        destination_dir_name = current_dir;
    else
    {
        backward_stack.push_back(current_dir);
        destination_dir_name = get_absolute_path(parameters[1]);
    }
    string destination_file_name = destination_dir_name + "/" + new_file_name;
    // cout << "creating file - " << new_file_name << endl;
    // cout << "appended to dest dir - " << destination_file_name << endl;
    FILE *f = fopen(destination_file_name.c_str(), "w+");
    fclose(f);
    populate_files_list(destination_dir_name.c_str());
    print_files_list("Command Mode", "Succesfully created new file! Displaying destination directory now.");
}

void command_create_dir(vector<string> parameters)
{
    // error handling
    if (parameters.size() != 3)
    {
        print_files_list("Command Mode", "Error, Must need atleast two arguemnts");
        return;
    }
    parameters.erase(parameters.begin());
    string new_dir_name = parameters[0];
    string destination_name;
    if (parameters.size() == 1)
        destination_name = current_dir;
    else
    {
        backward_stack.push_back(current_dir);
        destination_name = get_absolute_path(parameters[1]);
    }
    string destination_dir_name = destination_name + "/" + new_dir_name;
    mkdir(destination_dir_name.c_str(), 0777);
    populate_files_list(destination_name.c_str());
    print_files_list("Command Mode", "Succesfully created new directory! Displaying destination directory now.");
}

void command_goto(vector<string> parameters)
{
    backward_stack.push_back(current_dir);
    string path = get_absolute_path(parameters[1]);
    char *path_char = new char[path.length() + 1];
    strcpy(path_char, path.c_str());
    populate_files_list(path_char);
    print_files_list("Command Mode", "Navigating to " + path);
}

void command_mode()
{
    string statement;
    while (true)
    {
        getline(cin, statement);
        if (statement[0] == 27)
        {
            enableRawMode();
            clrscr();
            print_files_list();
            break;
        }
        else
        {
            vector<string> parameters;
            string word = "";
            for (auto x : statement)
            {
                if (x == ' ')
                {
                    parameters.push_back(word);
                    word = "";
                }
                else
                {
                    word = word + x;
                }
            }
            parameters.push_back(word);
            if (parameters[0] == "copy")
            {
                command_copy(parameters);
            }
            else if (parameters[0] == "move")
            {
                command_move(parameters);
            }
            else if (parameters[0] == "rename")
            {
                command_rename(parameters);
            }
            else if (parameters[0] == "create_file")
            {
                command_create_file(parameters);
            }
            else if (parameters[0] == "create_dir")
            {
                command_create_dir(parameters);
            }
            else if (parameters[0] == "goto")
            {
                // error handling
                if (parameters.size() != 2)
                {
                    print_files_list("Command Mode", "Error, Must have one arguemnt");
                    continue;
                }
                command_goto(parameters);
            }
            else if (parameters[0] == "search")
            {
                // error handling
                if (parameters.size() != 2)
                {
                    print_files_list("Command Mode", "Error, Must have one arguemnt");
                    continue;
                }
                if (command_search(parameters[1], current_dir))
                    print_files_list("Command Mode", "Found " + parameters[1]);
                else
                    print_files_list("Command Mode", "Did not found " + parameters[1]);
            }
            else if (parameters[0] == "delete_file")
            {
                // error handling
                if (parameters.size() != 2)
                {
                    print_files_list("Command Mode", "Error, Must have one arguemnt");
                    continue;
                }
                parameters.erase(parameters.begin());
                command_delete_file(parameters[0]);
            }
            else if (parameters[0] == "delete_dir")
            {
                // error handling
                if (parameters.size() != 2)
                {
                    print_files_list("Command Mode", "Error, Must have one arguemnt");
                    continue;
                }
                parameters.erase(parameters.begin());
                command_delete_dir(parameters[0]);
            }
            else
                print_files_list("Command Mode", "Error, the command you have entered is not available.");
            // char *path = new char[current_dir.length() + 1];
            // strcpy(path, current_dir.c_str());
            // populate_files_list(path);
            // print_files_list("Command Mode");
        }
    }
}

int main(void)
{
    enableRawMode();
    clrscr();
    struct passwd *pw = getpwuid(getuid());
    char *home = pw->pw_dir;
    home_dir = home;
    populate_files_list();
    signal(SIGWINCH, resize);
    print_files_list();
    char key;
    while (read(STDIN_FILENO, &key, 1) == 1 && key != 'q')
    {
        switch (key)
        {
        case 65:
            move_up();
            break;
        case 66:
            move_down();
            break;
        case 'h':
            go_to_home();
            break;
        case 67:
            go_forward();
            break;
        case 68:
            go_backward();
            break;
        case 127:
            go_to_parent();
            break;
        case 10:
            click();
            break;
        case ':':
            print_files_list("Command Mode");
            disableRawMode();
            command_mode();
            break;
        default:
            break;
        }
    }
    atexit(clrscr);
    atexit(disableRawMode);
    return 0;
}