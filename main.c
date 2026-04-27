#include <stdio.h>
#include <sqlite3.h>
#include <string.h>



// Callback function that prints todo list when ls command is entered 
int callback(void *data, int argc, char **argv, char **azColName) {
    if (argv[0]) {
        printf("[] %s", argv[0]);
    }
    return 0;
}

// Takes the input from user and give resulting output
void parse_input(char input_text[100], sqlite3 *db) {
    char *errMsg = 0;

    // List todo 
    if (strcmp(input_text, "ls\n") == 0) {
        char *get_task = "SELECT  Task FROM todolist WHERE Status=1";
        int rc = sqlite3_exec(db, get_task, callback, 0, &errMsg);
        if (rc != SQLITE_OK) {
            printf("SQL error: %s\n", errMsg);
            sqlite3_free(errMsg);
        }
    }
    
    // Help command list
    else if(strcmp(input_text, "h\n")==0){
        printf("ls : list todo task \n add <name of task> : to add task to todo list \n rm <name of task>: to delete task from todo list\n");
    }

    // Add todo 
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

        // Remove trailing space
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

    // Remove todo task
    else if (strncmp(input_text, "rm", 2) == 0) {

        sqlite3_stmt *stmt;

        // ---- extract query after "rm " ----
        char result[100] = "";

        char *token = strtok(input_text, " "); // skip "rm"
        token = strtok(NULL, " ");

        while (token != NULL) {
            strcat(result, token);
            strcat(result, " ");
            token = strtok(NULL, " ");
        }

        // remove trailing space
        if (strlen(result) > 0 && result[strlen(result) - 1] == ' ') {
            result[strlen(result) - 1] = '\0';
        }

        // Finding best match using fuzzy search
        const char *sql =
            "SELECT id, Task, fuzzy_jarowin(Task, ?) AS score "
            "FROM todolist "
            "ORDER BY score DESC "
            "LIMIT 1;";

        if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
            printf("Prepare failed\n");
            return;
        }

        sqlite3_bind_text(stmt, 1, result, -1, SQLITE_TRANSIENT);

        int best_id = -1;
        char best_task[200];
        double best_score = 0.0;

        if (sqlite3_step(stmt) == SQLITE_ROW) {
            best_id = sqlite3_column_int(stmt, 0);
            strcpy(best_task, (const char *)sqlite3_column_text(stmt, 1));
            best_score = sqlite3_column_double(stmt, 2);
        }

        sqlite3_finalize(stmt);

        // Check confidence
        if (best_id == -1 || best_score < 0.7) {
            printf("No close match found.\n");
            return;
        }


        // Delete by id
        const char *del_sql = "DELETE FROM todolist WHERE id = ?;";

        sqlite3_prepare_v2(db, del_sql, -1, &stmt, NULL);
        sqlite3_bind_int(stmt, 1, best_id);

        if (sqlite3_step(stmt) != SQLITE_DONE) {
            printf("Delete failed\n");
        } else {
            printf("Deleted: %s\n", best_task);
        }

        sqlite3_finalize(stmt);
    }
}



int main(){

    sqlite3 *db; // Create sqlite3 object
    char* errmsg;
    
    // Open database connection 
    int rc = sqlite3_open("todo.db", &db);
    if(rc != SQLITE_OK){
        printf("DB file created successfully %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }

    // Creat tabale in database
    char* create_table_command = "create table if not exists todolist(Id Integer Primary Key AUTOINCREMENT, Task Text, Topic Text, Status Integer)";
    rc = sqlite3_exec(db, create_table_command, 0, 0, &errmsg);
    if (rc != SQLITE_OK) {
        sqlite3_close(db);
        return 1;
    }

    // Config to load extension
    sqlite3_db_config(db,SQLITE_DBCONFIG_ENABLE_LOAD_EXTENSION,1, 0);
    sqlite3_enable_load_extension(db, 1);

    // add entension
    char *err = NULL;
    if (sqlite3_load_extension(db, "./fuzzy.dylib", 0, &err) != SQLITE_OK) {
        printf("Failed to load extension: %s\n", err);
        sqlite3_free(err);
        return 1;
    }

    char input_text[100];

    while(1){
        printf("\033[1;36m:>\033[0m ");

        fgets(input_text, sizeof(input_text), stdin);

        if (strcmp(input_text, "q\n") == 0) {
            break;
        }
        // If not exit parse input 
        parse_input(input_text, db);

    }
}
