

std::tuple<int, int, int, std::string> // score, row, column, alignment
smith_waterman( 
    std::string &reference, std::string &haplotype, //string read_BQ,
    int &match_score, int &mismatch_score, int &gap_open, int &gap_extend) {
    // std::cout << reference << std::endl;
    // std::cout << haplotype << std::endl;
    int align_scores[haplotype.length()+1][reference.length()+1] = {};
    int insert_scores[haplotype.length()+1][reference.length()+1] = {};
    int delete_scores[haplotype.length()+1][reference.length()+1] = {};
    
    int align_directions[haplotype.length()][reference.length()] = {};
    int insert_directions[haplotype.length()][reference.length()] = {};
    int delete_directions[haplotype.length()][reference.length()] = {};

    int weight = 0;
    int temp_align_score = 0;
    int temp_insert_score = 0;
    int temp_delete_score = 0;
    int highest_score = 0;
    int score_row = 0;
    int score_col = 0;
    int last_direction = 0;
    int highest_row = 0;
    int highest_col = 0;

    std::string alignment = "";

    std::queue <int>            SNP_or_INDEL_buffer;
    std::queue <std::string>    REF_buffer;
    std::queue <std::string>    ALT_buffer;
    std::queue <int>            allele_local_address_buffer;


    char direction_matrix[haplotype.length()][reference.length()];
    for (int i = 0; i < haplotype.length(); ++i)
    {
        for (int j=0; j<reference.length(); ++j)
        {
            direction_matrix[i][j] = 'O';
        }
    }

    for (int i=0; i<haplotype.length(); ++i)
    {
        for (int j=0; j<reference.length(); ++j)
        {
            // align scores operation
            if (haplotype[i]==reference[j])
            {
                weight = match_score;
            }
            else
            {
                weight = mismatch_score;
            }
            temp_align_score  = align_scores[i][j]  + weight;
            temp_insert_score = insert_scores[i][j] + weight;
            temp_delete_score = delete_scores[i][j] + weight;
            if (temp_delete_score > temp_align_score && temp_delete_score > temp_insert_score && temp_delete_score > 0)
            {
                align_scores[i+1][j+1] = temp_delete_score;
                align_directions[i][j] = 3;
            }
            else if (temp_insert_score > temp_align_score && temp_insert_score > 0)
            {
                align_scores[i+1][j+1] = temp_insert_score;
                align_directions[i][j] = 1;
            }
            else if (temp_align_score > 0)
            {
                align_scores[i+1][j+1] = temp_align_score;
                align_directions[i][j] = 2;
            }
            else
            {
                align_scores[i+1][j+1] = 0;
                align_directions[i][j] = 0;
            }

            
            // insert scores operation
            temp_align_score  = align_scores[i][j+1]  + gap_open;
            temp_insert_score = insert_scores[i][j+1] + gap_extend;
            if (temp_insert_score > temp_align_score && temp_insert_score > 0)
            {
                insert_scores[i+1][j+1] = temp_insert_score;
                insert_directions[i][j] = 1;
            }
            else if (temp_align_score > 0)
            {
                insert_scores[i+1][j+1] = temp_align_score;
                insert_directions[i][j] = 2;
            }
            else
            {
                insert_scores[i+1][j+1] = 0;
                insert_directions[i][j] = 0;
            }


            // delete scores operation
            temp_align_score  = align_scores[i+1][j]  + gap_open;
            temp_delete_score = delete_scores[i+1][j] + gap_extend;
            if (temp_delete_score > temp_align_score && temp_delete_score > 0)
            {
                delete_scores[i+1][j+1] = temp_delete_score;
                delete_directions[i][j] = 3;
            }
            else if (temp_align_score > 0)
            {
                delete_scores[i+1][j+1] = temp_align_score;
                delete_directions[i][j] = 2;
            }
            else
            {
                delete_scores[i+1][j+1] = 0;
                delete_directions[i][j] = 0;
            }


            // scores operation
            temp_align_score  = align_scores[i+1][j+1];
            temp_insert_score = insert_scores[i+1][j+1];
            temp_delete_score = delete_scores[i+1][j+1];
            if (temp_delete_score > temp_align_score && temp_delete_score > temp_insert_score && temp_delete_score > 0)
            {
                if (temp_delete_score > highest_score)
                {
                    highest_score = temp_delete_score;
                    highest_row = i;
                    highest_col = j;
                    last_direction = 3;
                }
            }
            else if (temp_insert_score > temp_align_score && temp_insert_score > 0)
            {
                if (temp_insert_score > highest_score)
                {
                    highest_score = temp_insert_score;
                    highest_row = i;
                    highest_col = j;
                    last_direction = 1;
                }
            }
            else if (temp_align_score > 0)
            {
                if (temp_align_score > highest_score)
                {
                    highest_score = temp_align_score;
                    highest_row = i;
                    highest_col = j;
                    last_direction = 2;
                }
            }
        }
        
    } // end of dynamic programming

    score_row = highest_row;
    score_col = highest_col;    
    ////////////////////// back trace //////////////////////
    // printf("back trace\n\n");
    int temp_direction;
    bool insertion_not_pushed = 0;
    bool deletion_not_pushed = 0;
    std::string temp_REF = "";
    std::string temp_ALT = "";
    char reference_base;
    char haplotype_base;
    while(true)
    {
        reference_base = reference[highest_col];
        haplotype_base = haplotype[highest_row];

        // printf("last_direction = %d\t", last_direction);


        if (last_direction == 2) // align
        {
            
            // std::cout << "local address " << highest_row << " (ref, alt) = (" << reference_base << ", " << haplotype_base << ")\n";
            if (reference_base != haplotype_base)
            {
                // convert char to string
                temp_REF = reference_base;
                temp_ALT = haplotype_base;
                
                // add variant to vector
                SNP_or_INDEL_buffer.push(2);
                REF_buffer.push(temp_REF);
                ALT_buffer.push(temp_ALT);
                allele_local_address_buffer.push(highest_col);

                temp_REF = "";
                temp_ALT = "";
                insertion_not_pushed = 0;
                deletion_not_pushed = 0;
            }
            else 
            {
                // check normalization
                if (insertion_not_pushed == 1) // insertion
                {
                    if (temp_ALT.back() == reference_base)
                    {
                        temp_ALT.pop_back();
                        temp_ALT = haplotype_base + temp_ALT;
                    }
                    else
                    {
                        // assert(deletion_not_pushed == 0);
                        temp_REF = reference_base + temp_REF;
                        temp_ALT = haplotype_base + temp_ALT;
                        // add variant to vector
                        SNP_or_INDEL_buffer.push(1);
                        REF_buffer.push(temp_REF);
                        ALT_buffer.push(temp_ALT);
                        allele_local_address_buffer.push(highest_col);

                        temp_REF = "";
                        temp_ALT = "";
                        insertion_not_pushed = 0;
                    }
                }
                else if (deletion_not_pushed == 1) // deletion
                {
                    if (temp_REF.back() == haplotype_base)
                    {
                        temp_REF.pop_back();
                        temp_REF = reference_base + temp_REF;
                    }
                    else
                    {
                        // assert(insertion_not_pushed == 0);
                        temp_REF = reference_base + temp_REF;
                        temp_ALT = haplotype_base + temp_ALT;
                        // add variant to vector
                        SNP_or_INDEL_buffer.push(3);
                        REF_buffer.push(temp_REF);
                        ALT_buffer.push(temp_ALT);
                        allele_local_address_buffer.push(highest_col);

                        temp_REF = "";
                        temp_ALT = "";

                        deletion_not_pushed = 0;
                    }
                }
            }

            last_direction = align_directions[highest_row][highest_col];
            if (last_direction!=0) alignment = '2' + alignment;
            // update index
            highest_row--;
            highest_col--;

        }
        else if (last_direction == 1) // insertion, check normalization
        {
            
            // assert(deletion_not_pushed == 0);
            if (deletion_not_pushed == 1) {
                
                temp_REF = reference_base + temp_REF;
                temp_ALT = reference_base + temp_ALT;
                // add variant to vector
                SNP_or_INDEL_buffer.push(3);
                REF_buffer.push(temp_REF);
                ALT_buffer.push(temp_ALT);
                allele_local_address_buffer.push(highest_col);

                temp_REF = "";
                temp_ALT = "";
                deletion_not_pushed = 0;
            }


            temp_ALT = haplotype_base + temp_ALT;

            last_direction = insert_directions[highest_row][highest_col];
            if (last_direction!=0) alignment = '1' + alignment;
            // assert(last_direction != 3);

            // update index
            highest_row--;

            insertion_not_pushed = 1;
        }
        else if (last_direction == 3)
        {
            
            // assert(insertion_not_pushed == 0);
            if (insertion_not_pushed == 1) {
                
                temp_REF = reference_base + temp_REF;
                temp_ALT = reference_base + temp_ALT;
                // add variant to vector
                SNP_or_INDEL_buffer.push(1);
                REF_buffer.push(temp_REF);
                ALT_buffer.push(temp_ALT);
                allele_local_address_buffer.push(highest_col);

                temp_REF = "";
                temp_ALT = "";
                insertion_not_pushed = 0;
            }

            temp_REF = reference_base + temp_REF;
            
            last_direction = delete_directions[highest_row][highest_col];
            if (last_direction!=0) alignment = '3' + alignment;
            // assert(last_direction != 1);

            // update index
            highest_col--;

            deletion_not_pushed = 1;
        }

        // printf("(row, col) = (%d, %d)\n", highest_row, highest_col);

        // terminate while loop
        if (last_direction==0 || highest_row<0 || highest_col<0)
        {
            break;
        }
    }

    

    return std::make_tuple(highest_score, score_row, score_col, alignment);
}