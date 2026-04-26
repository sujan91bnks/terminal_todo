#include <stdio.h>
#include <sqlite3.h>
#include <string.h>


int callback(void *data, int argc, char **argv, char **azColName) {
        if (argv[0]) {
        printf("[] %s", argv[0]);
    }
    return 0;
}

void parse_input(char input_text[100], sqlite3 *db) {
    char *errMsg = 0;

    if (strcmp(input_text, "ls\n") == 0) {

        char *get_task = "SELECT  Task FROM todolist WHERE Status=1";

        int rc = sqlite3_exec(db, get_task, callback, 0, &errMsg);

        if (rc != SQLITE_OK) {
            printf("SQL error: %s\n", errMsg);
            sqlite3_free(errMsg);
        }
    }

    // ADD command
    else if (strncmp(input_text, "add", 3) == 0) {

        sqlite3_stmt *stmt;

        char result[100] = "";

        char *token = strtok(input_text, " "); // skip "add"

        token = strtok(NULL, " ");

        while (token != NULL) {
            strcat(result, token);
            strcat(result, " ");
            token = strtok(NULL, " ");
        }
        if (strlen(result) > 0) {
            result[strlen(result) - 1] = '\0';
        }

        char *add_command =
            "INSERT INTO todolist(Task, Topic, Status) VALUES(?, 'maintopic', 1)";

        sqlite3_prepare_v2(db, add_command, -1, &stmt, NULL);

        sqlite3_bind_text(stmt, 1, result, -1, SQLITE_TRANSIENT);

        // execute
        if (sqlite3_step(stmt) != SQLITE_DONE) {
            printf("Insert failed\n");
        }

        sqlite3_finalize(stmt);
    }
    else if (strncmp(input_text, "rm",2) == 0) {
    sqlite3_stmt *stmt;

    char result[100] = "";

    char *token = strtok(input_text, " "); // skip "del"
    token = strtok(NULL, " ");

    while (token != NULL) {
        strcat(result, token);
        strcat(result, " ");
        token = strtok(NULL, " ");
    }
    if (strlen(result) > 0 && result[strlen(result) - 1] == ' ') {
    result[strlen(result) - 1] = '\0';
}

    char *delete_cmd = "DELETE FROM todolist WHERE Task LIKE ?";

    sqlite3_prepare_v2(db, delete_cmd, -1, &stmt, NULL);

    // add % for partial match
    char pattern[120];
    sprintf(pattern, "%%%s%%", result);

    sqlite3_bind_text(stmt, 1, pattern, -1, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        printf("Delete failed\n");
    } else {
        printf("Deleted matching tasks\n");
    }

    sqlite3_finalize(stmt);
}
}




int main(){
    // Status 0 means completed 
    // 1 means not completed
    
    
    sqlite3 *db; // Create sqlite3 object
    char* errmsg;

    // Open database connection 
    int rc = sqlite3_open("todo.db", &db);

    if(rc != SQLITE_OK){
        printf("DB file created successfully %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }

    char* create_table_command = "create table if not exists todolist(Id Integer Primary Key AUTOINCREMENT, Task Text, Topic Text, Status Integer)";
    rc = sqlite3_exec(db, create_table_command, 0, 0, &errmsg);
    if (rc != SQLITE_OK) {
        sqlite3_close(db);
        return 1;
    }


    char input_text[100];

    while(1){
        //printf(":> ");
        printf("\033[1;36m:>\033[0m ");
        fgets(input_text, sizeof(input_text), stdin);
        if (strcmp(input_text, "q\n") == 0) {
            // Ansi escpe characte to have Magenta color in terminal 
            printf("\033[35mFinished crap can be edited. Unfinished greatness languishes forever. Do it!!!\033[0m");
            break;
        }
        // If not exit parse input 
        parse_input(input_text, db);

    }
}
