#include "file_handler.h"

void read_into_buffer(file_handler *fh, bool target_buffer);

file_handler *create_file_handler(char *filename, int buffer_size, int max_retract_margin)
{
    assert((buffer_size % 2) == 0, "buffer size of file handler is even number");
    assert(max_retract_margin < (buffer_size / 2), "max retract margin is less than half buffer size");
    file_handler *fh = (file_handler *)calloc(1, sizeof(file_handler));
    fh->line_num = 1;
    fh->char_num = 1;
    fh->buffer_size = buffer_size;
    fh->max_retract_margin = max_retract_margin;
    fh->active_buffer = false;
    fh->file_ended = false;
    fh->stream_ended = false;
    fh->buffer0 = (char *)calloc(buffer_size / 2, sizeof(char));
    fh->buffer1 = (char *)calloc(buffer_size / 2, sizeof(char));
    fh->num_retractable = 0;
    fh->idx = 0;
    fh->filename = (char *)calloc(strlen(filename) + 1, sizeof(char));
    strcpy(fh->filename, filename);
    fh->char_num_stack = create_stack();
    fh->fptr = fopen(fh->filename, "r");
    assert((fh->fptr != NULL), "file opened successfully");
    read_into_buffer(fh, false);
    return fh;
}

void read_into_buffer(file_handler *fh, bool target_buffer)
{
    if (fh->stream_ended)
        return;
    if (fh->file_ended)
        return;
    char *buffer;
    if (target_buffer == false)
    {
        buffer = fh->buffer0;
    }
    else if (target_buffer == true)
    {
        buffer = fh->buffer1;
    }
    char c;
    int idx = 0;
    do
    {
        if (fscanf(fh->fptr, "%c", &c) == EOF)
        {
            fh->file_ended = true;
            buffer[idx] = EOF;
            idx++;
            break;
        }
        else
        {
            buffer[idx] = c;
            idx++;
        }
    } while (idx < (fh->buffer_size / 2));
    fh->last_populated_buffer = target_buffer;
}

bool needs_repopulation(file_handler *fh)
{
    if (fh->stream_ended)
        return false;
    if (fh->file_ended)
        return false;
    if (fh->active_buffer == !fh->last_populated_buffer)
        return false;
    if (fh->idx < fh->max_retract_margin)
        return false;
    return true;
}

char get_char(file_handler *fh)
{
    if (fh->stream_ended)
        return EOF;
    char *buffer;
    if (fh->active_buffer == false)
    {
        buffer = fh->buffer0;
    }
    else if (fh->active_buffer == true)
    {
        buffer = fh->buffer1;
    }
    char c = buffer[fh->idx];
    if (c == EOF)
    {
        fh->stream_ended = true;
        return c;
    }
    fh->idx = fh->idx + 1;
    if (fh->idx == (fh->buffer_size / 2))
    {
        fh->active_buffer = !fh->active_buffer;
        fh->idx = 0;
    }
    if (c == '\n')
    {
        fh->line_num = fh->line_num + 1;
        int *temp_char_num = calloc(1, sizeof(int));
        *temp_char_num = fh->char_num;
        push(fh->char_num_stack, (void *)temp_char_num);
        temp_char_num = NULL;
        fh->char_num = 1;
    }
    else
    {
        fh->char_num = fh->char_num + 1;
    }
    if (fh->num_retractable < fh->max_retract_margin)
    {
        fh->num_retractable = fh->num_retractable + 1;
    }
    if (needs_repopulation(fh))
        read_into_buffer(fh, !(fh->active_buffer));
    return c;
}

void retract_char(file_handler *fh)
{
    fh->idx = fh->idx - 1;
    if (fh->idx == -1)
    {
        fh->idx = ((fh->buffer_size) / 2) - 1;
        fh->active_buffer = !fh->active_buffer;
    }
    char *buffer;
    if (fh->active_buffer == false)
    {
        buffer = fh->buffer0;
    }
    else if (fh->active_buffer == true)
    {
        buffer = fh->buffer1;
    }

    char c = buffer[fh->idx];
    if (c == EOF)
    {
        fh->stream_ended = false;
        fh->char_num = fh->char_num - 1;
    }
    else if (c == '\n')
    {
        fh->line_num = fh->line_num - 1;
        int *temp_char_num = (int *)pop(fh->char_num_stack);
        fh->char_num = *temp_char_num;
        free(temp_char_num);
        temp_char_num = NULL;
    }
    else
    {
        fh->char_num = fh->char_num - 1;
    }
    fh->num_retractable = fh->num_retractable - 1;
}

void retract(file_handler *fh, int num_ret)
{
    assert(num_ret <= fh->num_retractable && num_ret >= 0, "retraction amount requested is within bounds");
    while (num_ret > 0)
    {
        retract_char(fh);
        num_ret--;
    }
}

int get_line_num(file_handler *fh)
{
    return fh->line_num;
}

int get_char_num(file_handler *fh)
{
    return fh->char_num;
}

void destroy_file_handler(file_handler *fh)
{
    fclose(fh->fptr);
    free(fh->filename);
    free(fh->buffer0);
    free(fh->buffer1);
    int *temp_char_num;
    while (get_stack_size(fh->char_num_stack) > 0)
    {
        temp_char_num = (int *)pop(fh->char_num_stack);
        free(temp_char_num);
    }
    destroy_stack(fh->char_num_stack);
    free(fh);
}
