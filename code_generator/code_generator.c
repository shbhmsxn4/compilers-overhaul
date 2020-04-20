#include "code_generator.h"

code *create_empty_code()
{
    code *c = calloc(1, sizeof(code));
    c->max_str = 1;
    c->num_str = 0;
    c->str = (char **)calloc(1, sizeof(char *));
    return c;
}

void expand_code_space(code *c)
{
    assert(c->num_str == c->max_str, "code space expansion called correctly");
    c->str = realloc(c->str, 2 * (c->max_str) * sizeof(char *));
    if (c->str == NULL)
    {
        assert(false, "code space expanded successfully");
    }
    (c->max_str) *= 2;
}

void append_code(code *c, char *str)
{
    if (c->num_str == c->max_str)
    {
        expand_code_space(c);
    }
    assert(c->num_str < c->max_str, "code space sufficient for append");
    c->str[c->num_str] = str;
    (c->num_str)++;
}

void prepend_code(code *c, char *str)
{
    if (c->num_str == c->max_str)
    {
        expand_code_space(c);
    }
    assert(c->num_str < c->max_str, "code space sufficient for prepend");
    for (int i = c->num_str; i > 0; i--)
    {
        c->str[i] = c->str[i - 1];
    }
    c->str[0] = str;
    (c->num_str)++;
}

void stitch_code_append(tree_node *dst, tree_node *src)
{
    code *dst_code;
    code *src_code;
    ast_leaf *ldata;
    ast_node *data;

    data = (ast_node *)get_data(dst);
    // printf("dst: %d ", data->label.gms.nt);
    if (data->is_leaf)
    {
        ldata = (ast_leaf *)get_data(dst);
        dst_code = ldata->c;
    }
    else
    {
        dst_code = data->c;
    }

    data = (ast_node *)get_data(src);
    // printf("src: %d\n", data->label.gms.nt);
    if (data->is_leaf)
    {
        ldata = (ast_leaf *)get_data(src);
        src_code = ldata->c;
    }
    else
    {
        // printf("hey\n");
        src_code = data->c;
    }

    assert(src_code != NULL && dst_code != NULL, "stitching valid codes");

    for (int i = 0; i < src_code->num_str; i++)
    {
        append_code(dst_code, src_code->str[i]);
    }
}

void stitch_code_prepend(code *dst, code *src)
{
    for (int i = src->num_str - 1; i >= 0; i--)
    {
        prepend_code(dst, src->str[i]);
    }
}

bool data_in_edx(tree_node *n)
{
    ast_node *data = (ast_node *)get_data(n);
    if (data->is_leaf)
    {
        ast_leaf *ldata = (ast_leaf *)get_data(n);
        if (ldata->ltk->t == NUM)
        {
            return true;
        }
        else if (ldata->ltk->t == RNUM)
        {
            return true;
        }
        else if (ldata->ltk->t == TRUE)
        {
            return true;
        }
        else if (ldata->ltk->t == FALSE)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        if (!data->label.is_terminal && data->label.gms.nt == var_id_num)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
}

void generate_code(tree_node *n, hash_map *st, scope_node *curr_scope, label_gen *lg)
{
    ast_node *data = (ast_node *)get_data(n);
    ast_node *data2 = NULL;
    ast_leaf *ldata = NULL;
    tree_node *n2 = NULL;
    id_type it_temp = 0;
    terminal t_temp = 0;
    func_entry *f_entry = NULL;
    scope_node *temp_scope = NULL;
    char *label_temp = NULL;
    char *label_temp2 = NULL;
    int offset = 0;
    bool is_param = false;
    common_id_entry *entry = NULL;
    if (data->is_leaf)
    {
        // printf("hey\n");
        data = NULL;
        ldata = (ast_leaf *)get_data(n);
        switch (ldata->ltk->t)
        {
        case NUM:

            ldata->c = create_empty_code();
            append_code(ldata->c, "mov edx, ");
            append_code(ldata->c, itoa(ldata->ltk->nv.int_val, (char *)calloc(MAX_OFFSET_DIGS, sizeof(char)), 10));
            append_code(ldata->c, "\n");
            break;

        case RNUM:
            ldata->c = create_empty_code();
            // TODO
            break;

        case TRUE:
            ldata->c = create_empty_code();
            append_code(ldata->c, "mov edx, 1\n");
            break;

        case FALSE:
            ldata->c = create_empty_code();
            append_code(ldata->c, "mov edx, 0\n");
            break;

        case ID:
            ldata->c = create_empty_code();
            entry = find_id_for(ldata->ltk->lexeme, curr_scope, for_use, ldata->ltk->line_num);
            offset = entry->entry.var_entry->offset;
            is_param = entry->is_param;
            it_temp = entry->entry.var_entry->type;
            if (it_temp == integer)
            {
                if (is_param)
                {
                    append_code(ldata->c, "mov dx, [ebp + ");
                    append_code(ldata->c, itoa(offset, (char *)calloc(MAX_OFFSET_DIGS, sizeof(char)), 10));
                    append_code(ldata->c, "]\n");
                    append_code(ldata->c, "and edx, 00001111h\n");
                }
                else
                {
                    append_code(ldata->c, "mov dx, [esi + ");
                    append_code(ldata->c, itoa(offset, (char *)calloc(MAX_OFFSET_DIGS, sizeof(char)), 10));
                    append_code(ldata->c, "]\n");
                    append_code(ldata->c, "and edx, 00001111h\n");
                }
            }
            else if (it_temp == real)
            {
                if (is_param)
                {
                    append_code(ldata->c, "mov edx, [ebp + ");
                    append_code(ldata->c, itoa(offset, (char *)calloc(MAX_OFFSET_DIGS, sizeof(char)), 10));
                    append_code(ldata->c, "]\n");
                }
                else
                {
                    append_code(ldata->c, "mov edx, [esi + ");
                    append_code(ldata->c, itoa(offset, (char *)calloc(MAX_OFFSET_DIGS, sizeof(char)), 10));
                    append_code(ldata->c, "]\n");
                }
            }
            else if (it_temp == boolean)
            {
                if (is_param)
                {
                    append_code(ldata->c, "mov dl, [ebp + ");
                    append_code(ldata->c, itoa(offset, (char *)calloc(MAX_OFFSET_DIGS, sizeof(char)), 10));
                    append_code(ldata->c, "]\n");
                    append_code(ldata->c, "and edx, 00000011h\n");
                }
                else
                {
                    append_code(ldata->c, "mov dl, [esi + ");
                    append_code(ldata->c, itoa(offset, (char *)calloc(MAX_OFFSET_DIGS, sizeof(char)), 10));
                    append_code(ldata->c, "]\n");
                    append_code(ldata->c, "and edx, 00000011h\n");
                }
            }
            else if (it_temp == array)
            {
                assert(false, "cannot copy array type as ID into edx");
            }
            else
            {
                assert(false, "ID has valid id_type");
            }
            break;
        }
    }
    else
    {
        if (data->label.is_terminal)
        {
            switch (data->label.gms.t)
            {
            }
        }
        else
        {
            switch (data->label.gms.nt)
            {
            case program:
                data->c = create_empty_code();
                append_code(data->c, "global main\n");
                append_code(data->c, "extern scanf\n");
                append_code(data->c, "extern printf\n");
                append_code(data->c, "\n\nsection .text\n");
                n2 = get_child(n, 1);
                data2 = (ast_node *)get_data(n2);
                for (int i = 0; i < ll_num_nodes(data2->ll); i++)
                {
                    generate_code(ll_get(data2->ll, i), st, NULL, lg);
                    stitch_code_append(n, ll_get(data2->ll, i));
                    append_code(data->c, "\n");
                }
                n2 = get_child(n, 3);
                data2 = (ast_node *)get_data(n2);
                for (int i = 0; i < ll_num_nodes(data2->ll); i++)
                {
                    generate_code(ll_get(data2->ll, i), st, NULL, lg);
                    stitch_code_append(n, ll_get(data2->ll, i));
                    append_code(data->c, "\n");
                }
                append_code(data->c, "\n\nmain:\n");
                f_entry = fetch_from_hash_map(st, "program");
                int temp_width = f_entry->width;
                append_code(data->c, "sub esp, ");
                append_code(data->c, itoa(temp_width, (char *)calloc(MAX_WIDTH_DIGS, sizeof(char)), 10));
                append_code(data->c, "\n");
                append_code(data->c, "mov esi, esp\n");
                n2 = get_child(n, 2);
                data2 = (ast_node *)get_data(n2);
                generate_code(n2, st, ((func_entry *)fetch_from_hash_map(st, "program"))->local_scope, lg);
                stitch_code_append(n, n2);
                append_code(data->c, "add esp, ");
                append_code(data->c, itoa(temp_width, (char *)calloc(MAX_WIDTH_DIGS, sizeof(char)), 10));
                append_code(data->c, "\n");
                append_code(data->c, "ret\n");
                append_code(data->c, "\n\nsection .data\n");
                append_code(data->c, "truestr db \"true\", 0\n");
                append_code(data->c, "falsestr db \"false\", 0\n");
                append_code(data->c, "ptruestr db \"true\", 10, 0\n");
                append_code(data->c, "pfalsestr db \"false\", 10, 0\n");
                append_code(data->c, "fmtd db \"%d\", 0\n");
                append_code(data->c, "pfmtd db \"%d\", 10, 0\n");
                append_code(data->c, "fmtf db \"%f\", 0\n");
                append_code(data->c, "fmts db \"%s\", 0\n");
                append_code(data->c, "\n\nsection .bss\n");
                append_code(data->c, "inpt resd 1\n");

                break;

            case moduleDef:
                data->c = create_empty_code();
                n2 = get_child(n, 1);
                generate_code(n2, st, curr_scope, lg);
                stitch_code_append(n, n2);
                break;

            case statements:
                data->c = create_empty_code();
                for (int i = 0; i < ll_num_nodes(data->ll); i++)
                {
                    // printf("%d\n", i);
                    generate_code(ll_get(data->ll, i), st, curr_scope, lg);
                    stitch_code_append(n, ll_get(data->ll, i));
                }
                break;

            case declareStmt:
                data->c = create_empty_code();
                break;

            case input_stmt:
                data->c = create_empty_code();
                n2 = get_child(n, 0);
                ldata = (ast_leaf *)get_data(n2);
                assert(ldata->is_leaf, "input stmt has ID leaf as child");
                entry = find_id_for(ldata->ltk->lexeme, curr_scope, for_assign, ldata->ltk->line_num);
                assert(entry->is_array == false, "cant take array as argument for input stmt");
                offset = entry->entry.var_entry->offset;
                is_param = entry->is_param;
                it_temp = entry->entry.var_entry->type;
                if (it_temp == integer)
                {
                    append_code(data->c, "push dword inpt\n");
                    append_code(data->c, "push dword fmtd\n");
                    append_code(data->c, "call scanf\n");
                    append_code(data->c, "add esp, 8\n");
                    append_code(data->c, "mov edx, [inpt]\n");
                    if (is_param)
                    {
                        append_code(data->c, "mov [ebp + ");
                        append_code(data->c, itoa(offset, (char *)calloc(MAX_OFFSET_DIGS, sizeof(char)), 10));
                        append_code(data->c, "], dx\n");
                    }
                    else
                    {
                        append_code(data->c, "mov [esi + ");
                        append_code(data->c, itoa(offset, (char *)calloc(MAX_OFFSET_DIGS, sizeof(char)), 10));
                        append_code(data->c, "], dx\n");
                    }
                }
                else if (it_temp == real)
                {
                    // TODO verify float input strat
                    append_code(data->c, "push dword inpt\n");
                    append_code(data->c, "push dword fmtf\n");
                    append_code(data->c, "call scanf\n");
                    append_code(data->c, "add esp, 8\n");
                    append_code(data->c, "mov edx, [inpt]\n");
                    if (is_param)
                    {
                        append_code(data->c, "mov [ebp + ");
                        append_code(data->c, itoa(offset, (char *)calloc(MAX_OFFSET_DIGS, sizeof(char)), 10));
                        append_code(data->c, "], edx\n");
                    }
                    else
                    {
                        append_code(data->c, "mov [esi + ");
                        append_code(data->c, itoa(offset, (char *)calloc(MAX_OFFSET_DIGS, sizeof(char)), 10));
                        append_code(data->c, "], edx\n");
                    }
                }
                else if (it_temp == boolean)
                {
                    append_code(data->c, "push dword inpt\n");
                    append_code(data->c, "push dword fmtd\n");
                    append_code(data->c, "call scanf\n");
                    append_code(data->c, "add esp, 8\n");
                    append_code(data->c, "mov edx, [inpt]\n");
                    append_code(data->c, "cmp edx, 0\n");

                    label_temp = (char *)calloc(MAX_LABEL_LEN, sizeof(char));
                    get_label(lg, label_temp);
                    label_temp2 = (char *)calloc(MAX_LABEL_LEN, sizeof(char));
                    get_label(lg, label_temp2);

                    append_code(data->c, "je ");
                    append_code(data->c, label_temp);
                    append_code(data->c, "\n");

                    if (is_param)
                    {
                        append_code(data->c, "mov byte [ebp + ");
                    }
                    else
                    {
                        append_code(data->c, "mov byte [esi + ");
                    }
                    append_code(data->c, itoa(offset, (char *)calloc(MAX_OFFSET_DIGS, sizeof(char)), 10));
                    append_code(data->c, "], 1\n");
                    append_code(data->c, "jmp ");
                    append_code(data->c, label_temp2);
                    append_code(data->c, "\n");

                    append_code(data->c, label_temp);
                    append_code(data->c, ":\n");
                    if (is_param)
                    {
                        append_code(data->c, "mov byte [ebp + ");
                    }
                    else
                    {
                        append_code(data->c, "mov byte [esi + ");
                    }
                    append_code(data->c, itoa(offset, (char *)calloc(MAX_OFFSET_DIGS, sizeof(char)), 10));
                    append_code(data->c, "], 0\n");

                    append_code(data->c, label_temp2);
                    append_code(data->c, ":\n");
                }
                else
                {
                    assert(false, "input stmt accepts only NUM RNUM or BOOLEAN as argument");
                }
                break;

            case output_stmt:
                data->c = create_empty_code();
                n2 = get_child(n, 0);
                ldata = (ast_leaf *)get_data(n2);
                if (ldata->is_leaf == false)
                {
                    // it's var_id_num node
                    data2 = (ast_node *)get_data(n2);
                    generate_code(n2, st, curr_scope, lg);
                    stitch_code_append(n, n2);

                    n2 = get_child(n2, 0);
                    ldata = (ast_leaf *)get_data(n2);
                    entry = find_id_for(ldata->ltk->lexeme, curr_scope, for_use, ldata->ltk->line_num);
                    if (entry->is_array)
                    {
                        // TODO printing of array
                    }
                    else
                    {
                        it_temp = entry->entry.var_entry->type;
                        if (it_temp == integer)
                        {
                            append_code(data->c, "push edx\n");
                            append_code(data->c, "push dword pfmtd\n");
                            append_code(data->c, "call printf\n");
                            append_code(data->c, "add esp, 8\n");
                        }
                        else if (it_temp == real)
                        {
                            // TODO printing of floats
                        }
                        else if (it_temp == boolean)
                        {
                            append_code(data->c, "cmp edx, 0\n");
                            label_temp = (char *)calloc(MAX_LABEL_LEN, sizeof(char));
                            get_label(lg, label_temp);
                            label_temp2 = (char *)calloc(MAX_LABEL_LEN, sizeof(char));
                            get_label(lg, label_temp2);
                            append_code(data->c, "je ");
                            append_code(data->c, label_temp);
                            append_code(data->c, "\n");
                            append_code(data->c, "push dword ptruestr\n");
                            append_code(data->c, "call printf\n");
                            append_code(data->c, "add esp, 4\n");
                            append_code(data->c, "jmp ");
                            append_code(data->c, label_temp2);
                            append_code(data->c, "\n");
                            append_code(data->c, label_temp);
                            append_code(data->c, ":\n");
                            append_code(data->c, "push dword pfalsestr\n");
                            append_code(data->c, "call printf\n");
                            append_code(data->c, "add esp, 4\n");
                            append_code(data->c, label_temp2);
                            append_code(data->c, ":\n");
                        }
                        else
                        {
                            assert(false, "output stmt non arr variable is in NUM RNUM BOOLEAN");
                        }
                    }
                }
                else
                {
                    if (ldata->label.gms.t == TRUE)
                    {
                        append_code(data->c, "push dword ptruestr\n");
                        append_code(data->c, "call printf\n");
                        append_code(data->c, "add esp, 4\n");
                    }
                    else if (ldata->label.gms.t == FALSE)
                    {
                        append_code(data->c, "push dword pfalsestr\n");
                        append_code(data->c, "call printf\n");
                        append_code(data->c, "add esp, 4\n");
                    }
                    else if (ldata->label.gms.t == NUM)
                    {
                        append_code(data->c, "mov edx, ");
                        append_code(data->c, itoa(ldata->ltk->nv.int_val, (char *)calloc(MAX_INT_LEN, sizeof(char)), 10));
                        append_code(data->c, "\n");
                        append_code(data->c, "push edx\n");
                        append_code(data->c, "push dword pfmtd\n");
                        append_code(data->c, "call printf\n");
                        append_code(data->c, "add esp, 8\n");
                    }
                    else if (ldata->label.gms.t == RNUM)
                    {
                        // TODO printing of float literals
                    }
                    else
                    {
                        assert(false, "if output stmt child has leaf, then it's NUM/RNUM/TRUE/FALSE");
                    }
                }

                break;

            case assignmentStmt:
                data->c = create_empty_code();
                n2 = get_child(n, 0);
                ldata = (ast_leaf *)get_data(n2);
                n2 = get_child(n, 1);
                data2 = (ast_node *)get_data(n2);
                entry = find_id_for(ldata->ltk->lexeme, curr_scope, for_use, ldata->ltk->line_num);
                is_param = entry->is_param;
                if (entry->is_array)
                {
                    // TODO
                }
                else
                {
                    generate_code(n2, st, curr_scope, lg);
                    stitch_code_append(n, n2);
                    offset = entry->entry.var_entry->offset;
                    it_temp = entry->entry.var_entry->type;
                    if (it_temp == integer)
                    {
                        if (is_param)
                        {
                            append_code(data->c, "mov [ebp + ");
                            append_code(data->c, itoa(offset, (char *)calloc(MAX_OFFSET_DIGS, sizeof(char)), 10));
                            append_code(data->c, "], dx\n");
                        }
                        else
                        {
                            append_code(data->c, "mov [esi + ");
                            append_code(data->c, itoa(offset, (char *)calloc(MAX_OFFSET_DIGS, sizeof(char)), 10));
                            append_code(data->c, "], dx\n");
                        }
                    }
                    else if (it_temp == real)
                    {
                        if (is_param)
                        {
                            append_code(data->c, "mov [ebp + ");
                            append_code(data->c, itoa(offset, (char *)calloc(MAX_OFFSET_DIGS, sizeof(char)), 10));
                            append_code(data->c, "], edx");
                        }
                        else
                        {
                            append_code(data->c, "mov [esi + ");
                            append_code(data->c, itoa(offset, (char *)calloc(MAX_OFFSET_DIGS, sizeof(char)), 10));
                            append_code(data->c, "], edx");
                        }
                    }
                    else if (it_temp == boolean)
                    {
                        if (is_param)
                        {
                            append_code(data->c, "mov [ebp + ");
                            append_code(data->c, itoa(offset, (char *)calloc(MAX_OFFSET_DIGS, sizeof(char)), 10));
                            append_code(data->c, "], dl");
                        }
                        else
                        {
                            append_code(data->c, "mov [esi + ");
                            append_code(data->c, itoa(offset, (char *)calloc(MAX_OFFSET_DIGS, sizeof(char)), 10));
                            append_code(data->c, "], dl");
                        }
                    }
                    else
                    {
                        assert(false, "assignment stmt lvalue is num rnum bool or arr");
                    }
                }
                break;

            case var_id_num:
                data->c = create_empty_code();
                n2 = get_child(n, 0);
                ldata = (ast_leaf *)get_data(n2);
                entry = find_id_for(ldata->ltk->lexeme, curr_scope, for_use, ldata->ltk->line_num);
                offset = entry->entry.var_entry->offset;
                is_param = entry->is_param;
                if (entry->is_array)
                {
                    it_temp = array;
                }
                else
                {
                    it_temp = entry->entry.var_entry->type;
                }
                if (it_temp == integer)
                {
                    if (is_param)
                    {
                        append_code(data->c, "mov dx, [ebp + ");
                        append_code(data->c, itoa(offset, (char *)calloc(MAX_OFFSET_DIGS, sizeof(char)), 10));
                        append_code(data->c, "]\n");
                        append_code(data->c, "push eax\n");
                        append_code(data->c, "mov ax, dx\n");
                        append_code(data->c, "cwde\n");
                        append_code(data->c, "mov edx, eax\n");
                        append_code(data->c, "pop eax\n");
                    }
                    else
                    {
                        append_code(data->c, "mov dx, [esi + ");
                        append_code(data->c, itoa(offset, (char *)calloc(MAX_OFFSET_DIGS, sizeof(char)), 10));
                        append_code(data->c, "]\n");
                        append_code(data->c, "push eax\n");
                        append_code(data->c, "mov ax, dx\n");
                        append_code(data->c, "cwde\n");
                        append_code(data->c, "mov edx, eax\n");
                        append_code(data->c, "pop eax\n");
                    }
                }
                else if (it_temp == real)
                {
                    // TODO
                }
                else if (it_temp == boolean)
                {
                    if (is_param)
                    {
                        append_code(data->c, "mov dl, [ebp + ");
                        append_code(data->c, itoa(offset, (char *)calloc(MAX_OFFSET_DIGS, sizeof(char)), 10));
                        append_code(data->c, "]\n");
                        append_code(data->c, "push eax\n");
                        append_code(data->c, "mov al, dl\n");
                        append_code(data->c, "cbw\n");
                        append_code(data->c, "cwde\n");
                        append_code(data->c, "mov edx, eax\n");
                        append_code(data->c, "pop eax\n");
                    }
                    else
                    {
                        append_code(data->c, "mov dl, [esi + ");
                        append_code(data->c, itoa(offset, (char *)calloc(MAX_OFFSET_DIGS, sizeof(char)), 10));
                        append_code(data->c, "]\n");
                        append_code(data->c, "push eax\n");
                        append_code(data->c, "mov al, dl\n");
                        append_code(data->c, "cbw\n");
                        append_code(data->c, "cwde\n");
                        append_code(data->c, "mov edx, eax\n");
                        append_code(data->c, "pop eax\n");
                    }
                }
                else if (it_temp == array)
                {
                    // TODO
                }
                else
                {
                    assert(false, "var_id_num has id_type in integer real boolean array");
                }
                break;

            case lvalueIDStmt:
                data->c = create_empty_code();
                n2 = get_child(n, 0);
                data2 = (ast_node *)get_data(n2);
                generate_code(n2, st, curr_scope, lg);
                stitch_code_append(n, n2);
                if (data_in_edx(n2))
                {
                    // do nothing
                }
                else
                {
                    append_code(data->c, "mov edx, eax\n");
                }
                break;

            case lvalueARRStmt:
                data->c = create_empty_code();
                n2 = get_child(n, 1);
                data2 = (ast_node *)get_data(n2);
                generate_code(n2, st, curr_scope, lg);
                stitch_code_append(n, n2);
                if (data_in_edx(n2))
                {
                    // do nothing
                }
                else
                {
                    append_code(data->c, "mov edx, eax\n");
                }
                break;

            case unary_nt:
                data->c = create_empty_code();
                n2 = get_child(n, 1);
                data2 = (ast_node *)get_data(n2);
                generate_code(n2, st, curr_scope, lg);
                stitch_code_append(n, n2);
                if (data_in_edx(n2))
                {
                    append_code(data->c, "mov eax, edx");
                    n2 = get_child(n, 0);
                    ldata = (ast_leaf *)get_data(n2);
                    if (ldata->ltk->t == MINUS)
                    {
                        append_code(data->c, "not eax\n");
                        append_code(data->c, "inc eax\n");
                    }
                }
                else
                {
                    n2 = get_child(n, 0);
                    ldata = (ast_leaf *)get_data(n2);
                    if (ldata->ltk->t == MINUS)
                    {
                        append_code(data->c, "not eax\n");
                        append_code(data->c, "inc eax\n");
                    }
                }
                break;

            case logicalOp:
                data->c = create_empty_code();

                n2 = get_child(n, 2);
                data2 = (ast_node *)get_data(n2);
                generate_code(n2, st, curr_scope, lg);
                stitch_code_append(n, n2);
                if (data_in_edx(n2))
                {
                    append_code(data->c, "push edx\n");
                }
                else
                {
                    append_code(data->c, "push eax\n");
                }

                n2 = get_child(n, 0);
                data2 = (ast_node *)get_data(n2);
                generate_code(n2, st, curr_scope, lg);
                stitch_code_append(n, n2);
                if (data_in_edx(n2))
                {
                    append_code(data->c, "mov eax, edx\n");
                }
                append_code(data->c, "pop edx\n");

                n2 = get_child(n, 1);
                ldata = (ast_leaf *)get_data(n2);
                t_temp = ldata->ltk->t;
                if (t_temp == AND)
                {
                    append_code(data->c, "and eax, edx\n");
                }
                else if (t_temp == OR)
                {
                    append_code(data->c, "or eax, edx\n");
                }
                else
                {
                    assert(false, "logical op is AND or OR");
                }
                break;

            case relationalOp:
                data->c = create_empty_code();

                n2 = get_child(n, 2);
                data2 = (ast_node *)get_data(n2);
                generate_code(n2, st, curr_scope, lg);
                stitch_code_append(n, n2);
                if (data_in_edx(n2))
                {
                    append_code(data->c, "push edx\n");
                }
                else
                {
                    append_code(data->c, "push eax\n");
                }

                n2 = get_child(n, 0);
                data2 = (ast_node *)get_data(n2);
                generate_code(n2, st, curr_scope, lg);
                stitch_code_append(n, n2);
                if (data_in_edx(n2))
                {
                    append_code(data->c, "mov eax, edx\n");
                }
                append_code(data->c, "pop edx\n");

                n2 = get_child(n, 1);
                ldata = (ast_leaf *)get_data(n2);
                t_temp = ldata->ltk->t;
                label_temp = calloc(MAX_LABEL_LEN, sizeof(char));
                get_label(lg, label_temp);
                label_temp2 = calloc(MAX_LABEL_LEN, sizeof(char));
                get_label(lg, label_temp2);
                append_code(data->c, "cmp eax, edx\n");
                if (t_temp == LT)
                {
                    append_code(data->c, "jl ");
                }
                else if (t_temp == LE)
                {
                    append_code(data->c, "jle ");
                }
                else if (t_temp == GT)
                {
                    append_code(data->c, "jg ");
                }
                else if (t_temp == GE)
                {
                    append_code(data->c, "jge ");
                }
                else if (t_temp == EQ)
                {
                    append_code(data->c, "je ");
                }
                else if (t_temp == NE)
                {
                    append_code(data->c, "jne ");
                }
                else
                {
                    assert(false, "relational op is one of LT LE GT GE EQ NE");
                }
                append_code(data->c, label_temp);
                append_code(data->c, "\n");
                append_code(data->c, "mov eax, 0\n");
                append_code(data->c, "jmp ");
                append_code(data->c, label_temp2);
                append_code(data->c, "\n");
                append_code(data->c, label_temp);
                append_code(data->c, ":\n");
                append_code(data->c, "mov eax, 1\n");
                append_code(data->c, label_temp2);
                append_code(data->c, ":\n");
                break;

            case for_loop:
                data->c = create_empty_code();
                bool for_loop_is_increasing = true;
                int for_range1, for_range2;

                n2 = get_child(n, 1);
                n2 = get_child(n2, 0);
                ldata = (ast_leaf *)get_data(n2);
                assert(ldata->label.is_terminal && ldata->label.gms.t == NUM, "range 1 for loop is NUM");
                assert(ldata->ltk->t == NUM, "range 1 for loop token is NUM");
                for_range1 = ldata->ltk->nv.int_val;

                n2 = get_child(n, 1);
                n2 = get_child(n2, 1);
                ldata = (ast_leaf *)get_data(n2);
                assert(ldata->label.is_terminal && ldata->label.gms.t == NUM, "range 2 for loop is NUM");
                assert(ldata->ltk->t == NUM, "range 2 for loop token is NUM");
                for_range2 = ldata->ltk->nv.int_val;

                if (for_range1 > for_range2)
                {
                    for_loop_is_increasing = false;
                    for_range2 -= 1;
                }
                else
                {
                    for_range2 += 1;
                }

                n2 = get_child(n, 0);
                ldata = (ast_leaf *)get_data(n2);
                entry = find_id_for(ldata->ltk->lexeme, curr_scope, for_use, ldata->ltk->line_num);
                offset = entry->entry.var_entry->offset;
                is_param = entry->is_param;
                assert(entry->entry.var_entry->type == integer, "for loop variable can only be an integer");

                append_code(data->c, "mov ecx, ");
                append_code(data->c, itoa(for_range1, (char *)calloc(MAX_INT_LEN, sizeof(char)), 10));
                append_code(data->c, "\n");
                if (is_param)
                {
                    append_code(data->c, "mov [ebp + ");
                }
                else
                {
                    append_code(data->c, "mov [esi + ");
                }
                append_code(data->c, itoa(offset, (char *)calloc(MAX_OFFSET_DIGS, sizeof(char)), 10));
                append_code(data->c, "], cx\n");

                label_temp = (char *)calloc(MAX_LABEL_LEN, sizeof(char));
                get_label(lg, label_temp);
                append_code(data->c, label_temp);
                append_code(data->c, ":\n");

                label_temp2 = (char *)calloc(MAX_LABEL_LEN, sizeof(char));
                get_label(lg, label_temp2);

                append_code(data->c, "mov cx, ");
                if (is_param)
                {
                    append_code(data->c, "[ebp + ");
                }
                else
                {
                    append_code(data->c, "[esi + ");
                }
                append_code(data->c, itoa(offset, (char *)calloc(MAX_OFFSET_DIGS, sizeof(char)), 10));
                append_code(data->c, "]\n");
                append_code(data->c, "cmp cx, ");
                append_code(data->c, itoa(for_range2, (char *)calloc(MAX_INT_LEN, sizeof(char)), 10));
                append_code(data->c, "\n");
                append_code(data->c, "je ");
                append_code(data->c, label_temp2);
                append_code(data->c, "\n");

                n2 = get_child(n, 3);
                data2 = (ast_node *)get_data(n2);
                generate_code(n2, st, curr_scope, lg);
                stitch_code_append(n, n2);

                append_code(data->c, "mov cx, ");
                if (is_param)
                {
                    append_code(data->c, "[ebp + ");
                }
                else
                {
                    append_code(data->c, "[esi + ");
                }
                append_code(data->c, itoa(offset, (char *)calloc(MAX_OFFSET_DIGS, sizeof(char)), 10));
                append_code(data->c, "]\n");
                if (for_loop_is_increasing)
                {
                    append_code(data->c, "inc cx\n");
                }
                else
                {
                    append_code(data->c, "dec cx\n");
                }
                append_code(data->c, "mov ");
                if (is_param)
                {
                    append_code(data->c, "[ebp + ");
                }
                else
                {
                    append_code(data->c, "[esi + ");
                }
                append_code(data->c, itoa(offset, (char *)calloc(MAX_OFFSET_DIGS, sizeof(char)), 10));
                append_code(data->c, "], cx\n");
                append_code(data->c, "jmp ");
                append_code(data->c, label_temp);
                append_code(data->c, "\n");
                append_code(data->c, label_temp2);
                append_code(data->c, ":\n");

                break;

            case while_loop:
                label_temp = (char *)calloc(MAX_LABEL_LEN, sizeof(char));
                get_label(lg, label_temp);
                label_temp2 = (char *)calloc(MAX_LABEL_LEN, sizeof(char));
                get_label(lg, label_temp2);
                data->c = create_empty_code();

                append_code(data->c, label_temp);
                append_code(data->c, ":\n");

                n2 = get_child(n, 0);
                data2 = get_data(n2);
                generate_code(n2, st, curr_scope, lg);
                stitch_code_append(n, n2);
                if (data_in_edx(n2))
                {
                    append_code(data->c, "cmp edx, 0\n");
                }
                else
                {
                    append_code(data->c, "cmp eax, 0\n");
                }

                append_code(data->c, "je ");
                append_code(data->c, label_temp2);
                append_code(data->c, "\n");

                n2 = get_child(n, 2);
                data2 = (ast_node *)get_data(n2);
                generate_code(n2, st, curr_scope, lg);
                stitch_code_append(n, n2);

                append_code(data->c, "jmp ");
                append_code(data->c, label_temp);
                append_code(data->c, "\n");

                append_code(data->c, label_temp2);
                append_code(data->c, ":\n");
                break;

            case op1:
                n2 = get_child(n, 1);
                ldata = (ast_leaf *)get_data(n2);
                if (ldata->label.gms.t == PLUS)
                {
                    data->c = create_empty_code();

                    n2 = get_child(n, 2);
                    data2 = (ast_node *)get_data(n2);
                    generate_code(n2, st, curr_scope, lg);
                    stitch_code_append(n, n2);
                    if (data_in_edx(n2))
                    {
                        append_code(data->c, "push edx\n");
                    }
                    else
                    {
                        append_code(data->c, "push eax\n");
                    }

                    n2 = get_child(n, 0);
                    data2 = (ast_node *)get_data(n2);
                    generate_code(n2, st, curr_scope, lg);
                    stitch_code_append(n, n2);
                    if (data_in_edx(n2))
                    {
                        append_code(data->c, "mov eax, edx\n");
                    }
                    append_code(data->c, "pop edx\n");

                    append_code(data->c, "add eax, edx\n");
                }
                else if (ldata->label.gms.t == MINUS)
                {

                    data->c = create_empty_code();

                    n2 = get_child(n, 2);
                    data2 = (ast_node *)get_data(n2);
                    generate_code(n2, st, curr_scope, lg);
                    stitch_code_append(n, n2);
                    if (data_in_edx(n2))
                    {
                        append_code(data->c, "push edx\n");
                    }
                    else
                    {
                        append_code(data->c, "push eax\n");
                    }

                    n2 = get_child(n, 0);
                    data2 = (ast_node *)get_data(n2);
                    generate_code(n2, st, curr_scope, lg);
                    stitch_code_append(n, n2);
                    if (data_in_edx(n2))
                    {
                        append_code(data->c, "mov eax, edx\n");
                    }
                    append_code(data->c, "pop edx\n");

                    append_code(data->c, "sub eax, edx\n");
                }
                break;

            case op2:
                n2 = get_child(n, 1);
                ldata = (ast_leaf *)get_data(n2);
                if (ldata->label.gms.t == MUL)
                {
                    data->c = create_empty_code();

                    n2 = get_child(n, 2);
                    data2 = (ast_node *)get_data(n2);
                    generate_code(n2, st, curr_scope, lg);
                    stitch_code_append(n, n2);
                    if (data_in_edx(n2))
                    {
                        append_code(data->c, "push edx\n");
                    }
                    else
                    {
                        append_code(data->c, "push eax\n");
                    }

                    n2 = get_child(n, 0);
                    data2 = (ast_node *)get_data(n2);
                    generate_code(n2, st, curr_scope, lg);
                    stitch_code_append(n, n2);
                    if (data_in_edx(n2))
                    {
                        append_code(data->c, "mov eax, edx\n");
                    }
                    append_code(data->c, "pop edx\n");

                    append_code(data->c, "imul dx\n");
                    append_code(data->c, "cwde\n");
                }
                else if (ldata->label.gms.t == DIV)
                {
                    data->c = create_empty_code();

                    n2 = get_child(n, 2);
                    data2 = (ast_node *)get_data(n2);
                    generate_code(n2, st, curr_scope, lg);
                    stitch_code_append(n, n2);
                    if (data_in_edx(n2))
                    {
                        append_code(data->c, "push edx\n");
                    }
                    else
                    {
                        append_code(data->c, "push eax\n");
                    }

                    n2 = get_child(n, 0);
                    data2 = (ast_node *)get_data(n2);
                    generate_code(n2, st, curr_scope, lg);
                    stitch_code_append(n, n2);
                    if (data_in_edx(n2))
                    {
                        append_code(data->c, "mov eax, edx\n");
                    }
                    append_code(data->c, "pop edx\n");

                    append_code(data->c, "push ebx\n");
                    append_code(data->c, "mov ebx, edx\n");
                    append_code(data->c, "mov edx, eax\n");
                    append_code(data->c, "shr edx, 16\n");
                    append_code(data->c, "idiv bx\n");
                    append_code(data->c, "cwde\n");
                    append_code(data->c, "pop ebx\n");
                }
                break;
            }
        }
    }
}

/*
CODE DELETED FROM END OF case output_stmt

if (ldata->ltk->t == NUM)
                {
                    append_code(data->c, "push inpt\n");
                    append_code(data->c, "push fmtd\n");
                    append_code(data->c, "call scanf\n");
                    append_code(data->c, "add esp, 8\n");
                    append_code(data->c, "mov edx, [inpt]\n");
                    if (is_param)
                    {
                        append_code(data->c, "mov [ebp + ");
                        append_code(data->c, itoa(offset, (char *)calloc(MAX_OFFSET_DIGS, sizeof(char)), 10));
                        append_code(data->c, "], edx\n");
                    }
                    else
                    {
                        append_code(data->c, "mov [esp + ");
                        append_code(data->c, itoa(offset, (char *)calloc(MAX_OFFSET_DIGS, sizeof(char)), 10));
                        append_code(data->c, "], edx\n");
                    }
                }
                else if (ldata->ltk->t == RNUM)
                {
                    append_code(data->c, "push inpt\n");
                    append_code(data->c, "push fmtf\n");
                    append_code(data->c, "call scanf\n");
                    append_code(data->c, "add esp, 8\n");
                    append_code(data->c, "mov edx, inpt\n");
                    if (is_param)
                    {
                        append_code(data->c, "mov [ebp + ");
                        append_code(data->c, itoa(offset, (char *)calloc(MAX_OFFSET_DIGS, sizeof(char)), 10));
                        append_code(data->c, "], edx\n");
                    }
                    else
                    {
                        append_code(data->c, "mov [esp + ");
                        append_code(data->c, itoa(offset, (char *)calloc(MAX_OFFSET_DIGS, sizeof(char)), 10));
                        append_code(data->c, "], edx\n");
                    }
                }
                else if (ldata->ltk->t == BOOLEAN)
                {
                }
                else
                {
                    assert(false, "input stmt accepts only NUM RNUM or BOOLEAN as argument");
                }

*/