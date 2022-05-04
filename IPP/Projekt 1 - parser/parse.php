<?php
/*
Name:           Michal Řezník
University:     Brno University of Technology
Faculty:        Information Technology
Subject:        IPP
Project:        parser.php
Date:           10.3.2021
*/


    ini_set('display_erros', 'STDERR');


    function Check_codes($string){
        $array = str_split($string);
        for($r = 0; $r <= strlen($string);$r++){
            if($array[$r] == "\\"){
                $r++;
                if($array[$r] == 1){
                    $r++;
                    if(is_numeric($array[$r])){
                        $r++;
                        if(is_numeric($array[$r])){
                            return;
                        }
                        else{
                            fwrite(STDERR, "Input code failure\n");
                            exit(23);
                        }
                    }
                    else{
                        fwrite(STDERR, "Input code failure\n");
                        exit(23);
                    }
                }
                else if($array[$r] == 0){
                    $r++;
                    if(is_numeric($array[$r])){
                        $r++;
                        if(is_numeric($array[$r])){
                            return;
                        }
                        else{
                            fwrite(STDERR, "Input code failure\n");
                            exit(23);
                        }
                    }
                    else{
                        fwrite(STDERR, "Input code failure\n");
                        exit(23);
                    }
                }
                else{
                    fwrite(STDERR, "Input code failure\n");
                    exit(23);
                }
            }
        }
    }
    $order = 1; //our order count

    //check for help case
    if($argc == 2 AND $argv[1] == "--help"){
        fwrite(STDOUT, "Usage : parse.php [tag] <Input_File\n");
        exit(0);
    }
    //check wrong number of arguments
    else if($argc != 1 OR $argc >2) {
        fwrite(STDERR,'Arguments failure use tag "--help" for usage options.'."\n");
        exit(10);
    }
    $head = false; //set variable for our head '.IPPcode21'
    fwrite(STDOUT,'<?xml version="1.0" encoding="UTF-8"?>'."\n");
    while($line = fgets(STDIN)){
        if($head == false){
            if($line == "\n" OR substr($line, 0,1)==='#'){ //letting be empty lines and comments 
                continue;
            }
            $position = strpos($line,'#'); 
            if($position !== false){
                $line = substr($line,0,$position); 
            }
            if(strtoupper($line) == ".IPPCODE21\n" OR strtoupper($line) == ".IPPCODE21"){ //not case sensitive so we must do uppercase whole string to compare them
                fwrite(STDOUT,'<program language="IPPcode21">'."\n");
                $head = true;
                continue;
            }
            else{           //missing head == error
                fwrite(STDERR,"Missing head\n");
                exit(21);
            }
        }
        if(strpos($line, "\n")){ //if there is new line after this we wil remove \n for easier work
            $cutted=substr_replace($line, "", -1);
        }           
        else{ //if therre is not a new line char we will let it be
            $cutted = $line;
        }
        //deleting comments from out strings
        $position = strpos($cutted,'#'); 
        if($position !== false){
            $cutted = substr($cutted,0,$position); 
        }
        //deleting empty elements from end of the string
        while(substr($cutted,-1) == " "){
            $cutted=substr_replace($cutted, "", -1);
        }
        while(substr($cutted,0,1) == " "){
            $cutted =substr($cutted,1);
        }
        //make array of words
        $exploted = preg_split("/[\s]+/", $cutted);
        //compare them by first word(name of command)
        switch(strtoupper($exploted[0])){
            case "DEFVAR":
                if(sizeof($exploted)!= 2){
                    fwrite(STDERR, "Input code failure\n");
                    exit(23);
                }
                fwrite(STDOUT,"\t<instruction order=".'"'.$order.'"'." opcode=".'"'.strtoupper($exploted[0]).'"'.">\n");
                $order++;
                if(preg_match("/^(LF|GF|TF)@[a-zA-Z_$\-&%*!?][a-zA-Z_$\-&*%!?$0-9]*$/", $exploted[1])){
                    fwrite(STDOUT, "\t\t <arg1 type=\"var\">".$exploted[1]."</arg1>\n");
                    fwrite(STDOUT,"\t</instruction>\n");
                    break;
                }
                else{
                    fwrite(STDERR, "Input code failure\n");
                    exit(23);
                }
            case "BREAK":
                if(sizeof($exploted)!= 1){
                    fwrite(STDERR, "Input code failure\n");
                    exit(23);
                }
                fwrite(STDOUT,"\t<instruction order=".'"'.$order.'"'." opcode=".'"'.strtoupper($exploted[0]).'"'."/>\n");
                $order++;
                break;
            case "DPRINT":
                if(sizeof($exploted)!= 2){
                    fwrite(STDERR, "Input code failure\n");
                    exit(23);
                }
                if((preg_match("/^int@[0-9][0-9]*$/", $exploted[1])) OR (preg_match("/^int@-[0-9][0-9]*$/", $exploted[1]))){
                    fwrite(STDOUT,"\t<instruction order=".'"'.$order.'"'." opcode=".'"'.strtoupper($exploted[0]).'"'.">\n");
                    $order++;
                    fwrite(STDOUT,"\t\t <arg1 type=\"int\">".substr($exploted[1],4)."</arg1>\n");
                    fwrite(STDOUT,"\t</instruction>\n");
                }
                else{
                    fwrite(STDERR, "Input code failure\n");
                    exit(23);
                }
                break;
            case "EXIT":
                if(sizeof($exploted)!= 2){
                    fwrite(STDERR, "Input code failure\n");
                    exit(23);
                }
                if(substr($exploted[1],0,4) != "int@"){
                    fwrite(STDERR, "Input code failure\n");
                    exit(23);
                }
                if(intval(substr($exploted[1],4)) > 49 AND intval(substr($exploted[1],4)) < 0){
                    fwrite(STDERR,"Input failure\n");
                    exit(23);
                }
                //TODO 0-49 check
                fwrite(STDOUT,"\t<instruction order=".'"'.$order.'"'." opcode=".'"'.strtoupper($exploted[0]).'"'.">\n");
                $order++;
                fwrite(STDOUT,"\t\t <arg1 type=\"int\">".substr($exploted[1],4)."</arg1>\n");
                fwrite(STDOUT,"\t</instruction>\n");
                break;
            case "JUMPIFEQ":
            case "JUMPIFNEQ":
                if(sizeof($exploted)!= 4){
                    fwrite(STDERR, "Input code failure\n");
                    exit(23);
                }
                fwrite(STDOUT,"\t<instruction order=".'"'.$order.'"'." opcode=".'"'.strtoupper($exploted[0]).'"'.">\n");
                $order++;
                if(preg_match("/^[a-zA-Z_\-$&%*!?][a-zA-Z_\-$&*%!?$0-9]*$/", $exploted[1])){
                    fwrite(STDOUT,"\t\t <arg1 type=\"label\">".$exploted[1]."</arg1>\n");
                }
                else{
                    fwrite(STDERR, "Input code failure\n");
                    exit(23);
                }
                if(preg_match("/^(LF|GF|TF)@[a-zA-Z_$\-#&*%!?][a-zA-Z_$\-#&%*!?$0-9]*$/", $exploted[2])){
                    fwrite(STDOUT,"\t\t <arg2 type=\"var\">".$exploted[2]."</arg2>\n");
                }
                else if(substr($exploted[2],0,7)=="string@"){
                    Check_codes(substr($exploted[2],7));
                    fwrite(STDOUT,"\t\t <arg2 type=\"string\">".htmlspecialchars(substr($exploted[2],7))."</arg2>\n");
                }
                else if((preg_match("/^int@[0-9][0-9]*$/", $exploted[2])) OR (preg_match("/^int@-[0-9][0-9]*$/", $exploted[2]))){
                    fwrite(STDOUT,"\t\t <arg2 type=\"int\">".substr($exploted[2],4)."</arg2>\n");
                }
                else if(preg_match("/^(bool)@[false|true]/", $exploted[2])){
                    fwrite(STDOUT,"\t\t <arg2 type=\"bool\">".substr($exploted[2],5)."</arg2>\n");
                }
                else if(preg_match("/^(nil)@(nil)$/", $exploted[2])){
                    fwrite(STDOUT,"\t\t <arg2 type=\"nil\">".substr($exploted[2],4)."</arg2>\n");
                }
                else{
                    fwrite(STDERR, "Input code failure\n");
                    exit(23);
                }

                if(preg_match("/^(LF|GF|TF)@[a-zA-Z_$\-#&%*!?][a-zA-Z_$\-#&*%!?$0-9]*$/", $exploted[3])){
                    fwrite(STDOUT,"\t\t <arg3 type=\"var\">".$exploted[3]."</arg3>\n");
                }
                else if(substr($exploted[3],0,7)=="string@"){
                    Check_codes(substr($exploted[3],7));
                    fwrite(STDOUT,"\t\t <arg3 type=\"string\">".htmlspecialchars(substr($exploted[3],7))."</arg3>\n");
                }
                else if((preg_match("/^int@[0-9][0-9]*$/", $exploted[3])) OR (preg_match("/^int@-[0-9][0-9]*$/", $exploted[3]))){
                    fwrite(STDOUT,"\t\t <arg3 type=\"int\">".substr($exploted[3],4)."</arg3>\n");
                }
                else if(preg_match("/^(bool)@[false|true]/", $exploted[3])){
                    fwrite(STDOUT,"\t\t <arg3 type=\"bool\">".substr($exploted[3],5)."</arg3>\n");
                }
                else if(preg_match("/^(nil)@[nil]$/", $exploted[3])){
                    fwrite(STDOUT,"\t\t <arg3 type=\"nil\">".substr($exploted[3],4)."</arg3>\n");
                }
                else{
                    fwrite(STDERR, "Input code failure\n");
                    exit(23);
                }
                fwrite(STDOUT,"\t</instruction>\n");
                break;
            case "JUMP":
                if(sizeof($exploted)!= 2){
                    fwrite(STDERR, "Input code failure\n");
                    exit(23);
                }
                fwrite(STDOUT,"\t<instruction order=".'"'.$order.'"'." opcode=".'"'.strtoupper($exploted[0]).'"'.">\n");
                $order++;
                if(preg_match('/^[a-zA-Z_$\-&%*!?][a-zA-Z_$\-&*%!?$0-9]*$/', $exploted[1])){
                    fwrite(STDOUT, "\t\t <arg1 type=\"label\">".$exploted[1]."</arg1>\n");
                    fwrite(STDOUT,"\t</instruction>\n");
                    break;
                }
                else{
                    fwrite(STDERR, "Input code failure\n");
                    exit(23);
                }
            case "LABEL":
                if(sizeof($exploted)!= 2){
                    fwrite(STDERR, "Input code failure\n");
                    exit(23);
                }
                fwrite(STDOUT,"\t<instruction order=".'"'.$order.'"'." opcode=".'"'.strtoupper($exploted[0]).'"'.">\n");
                $order++;
                if(is_numeric($exploted[1][0])){
                    fwrite(STDERR, "Input code failure\n");
                    exit(23);
                }
                if(preg_match("/^[a-zA-Z_$\-&*%!?$0-9]*$/", $exploted[1])){
                    fwrite(STDOUT,"\t\t <arg1 type=\"label\">".$exploted[1]."</arg1>\n");
                    fwrite(STDOUT,"\t</instruction>\n");
                    break;
                }
                else{
                    fwrite(STDERR, "Input code failure\n");
                    exit(23);
                }
            case "TYPE":
                if(sizeof($exploted)!= 3){
                    fwrite(STDERR, "Input code failure\n");
                    exit(23);
                }
                fwrite(STDOUT,"\t<instruction order=".'"'.$order.'"'." opcode=".'"'.strtoupper($exploted[0]).'"'.">\n");
                $order++;
                if(preg_match("/^(LF|GF|TF)@[a-zA-Z_$\-#&%*!?][a-zA-Z_$\-#&*%!?$0-9]*$/", $exploted[1])){
                    fwrite(STDOUT,"\t\t <arg1 type=\"var\">".$exploted[1]."</arg1>\n");
                }
                else{
                    fwrite(STDERR, "Input code failure\n");
                    exit(23);
                }
                if($exploted[2] == "nil@nil"){
                    fwrite(STDOUT,"\t\t <arg2 type=\"nil\">".substr($exploted[2],4)."</arg2>\n");
                    fwrite(STDOUT,"\t</instruction>\n");
                    break;
                }
                else if($exploted[2] == "bool@false" OR $exploted[2] == "bool@true"){
                    fwrite(STDOUT,"\t\t <arg2 type=\"bool\">".substr($exploted[2],5)."</arg2>\n");
                    fwrite(STDOUT,"\t</instruction>\n");
                    break;
                }
                else if((preg_match("/^int@[0-9][0-9]*$/", $exploted[2])) OR (preg_match("/^int@-[0-9][0-9]*$/", $exploted[2]))){
                    fwrite(STDOUT,"\t\t <arg2 type=\"int\">".substr($exploted[2],4)."</arg2>\n");
                }
                else if(preg_match("/^(LF|GF|TF)@[a-zA-Z_$\-#&%*!?][a-zA-Z_$\-#&*%!?$0-9]*$/", $exploted[2])){
                    fwrite(STDOUT,"\t\t <arg2 type=\"var\">".substr($exploted[2],3)."</arg2>\n");
                }
                else if(substr($exploted[2],0,7)=="string@"){
                    Check_codes(substr($exploted[2],7));
                    fwrite(STDOUT,"\t\t <arg2 type=\"string\">".htmlspecialchars(substr($exploted[2],7))."</arg2>\n");
                }
                else{
                    fwrite(STDERR, "Input code failure\n");
                    exit(23);
                }
                fwrite(STDOUT,"\t</instruction>\n");
                break;
            case "SETCHAR":
                if(sizeof($exploted)!= 4){
                    fwrite(STDERR, "Input code failure\n");
                    exit(23);
                }
                fwrite(STDOUT,"\t<instruction order=".'"'.$order.'"'." opcode=".'"'.strtoupper($exploted[0]).'"'.">\n");
                $order++;
                if(preg_match("/^(LF|GF|TF)@[a-zA-Z_$\-#&%*!?][a-zA-Z_$\-#&*%!?$0-9]*$/", $exploted[1])){
                    fwrite(STDOUT,"\t\t <arg1 type=\"var\">".$exploted[1]."</arg1>\n");
                }
                else{
                    fwrite(STDERR, "Input code failure\n");
                    exit(23);
                }
                if((preg_match("/^int@[0-9][0-9]*$/", $exploted[2])) OR (preg_match("/^int@-[0-9][0-9]*$/", $exploted[2]))){
                    fwrite(STDOUT,"\t\t <arg2 type=\"int\">".$exploted[2]."</arg2>\n");
                }
                else{
                    fwrite(STDERR, "Input code failure\n");
                    exit(23);
                }
                if(substr($exploted[3],0,7)=="string@"){
                    Check_codes(substr($exploted[3],7));
                    fwrite(STDOUT,"\t\t <arg3 type=\"string\">".htmlspecialchars(substr($exploted[3],7))."</arg3>\n");
                }
                else{
                    fwrite(STDERR, "Input code failure\n");
                    exit(23);
                }
                fwrite(STDOUT,"\t</instruction>\n");
                break;
            case "GETCHAR":
                if(sizeof($exploted)!= 4){
                    fwrite(STDERR, "Input code failure\n");
                    exit(23);
                }
                fwrite(STDOUT,"\t<instruction order=".'"'.$order.'"'." opcode=".'"'.strtoupper($exploted[0]).'"'.">\n");
                $order++;
                if(preg_match("/^(LF|GF|TF)@[a-zA-Z_$\-#&%*!?][a-zA-Z_$\-#&*%!?$0-9]*$/", $exploted[1])){
                    fwrite(STDOUT,"\t\t <arg1 type=\"var\">".$exploted[1]."</arg1>\n");
                }
                else{
                    fwrite(STDERR, "Input code failure\n");
                    exit(23);
                }
                if(substr($exploted[2],0,7)=="string@"){
                    Check_codes(substr($exploted[2],7));
                    fwrite(STDOUT,"\t\t <arg2 type=\"string\">".htmlspecialchars(substr($exploted[2],7))."</arg2>\n");
                }
                else{
                    fwrite(STDERR, "Input code failure\n");
                    exit(23);
                }
                if((preg_match("/^int@[0-9][0-9]*$/", $exploted[3])) OR (preg_match("/^int@-[0-9][0-9]*$/", $exploted[3]))){
                    fwrite(STDOUT,"\t\t <arg3 type=\"int\">".$exploted[3]."</arg3>\n");
                }
                else{
                    fwrite(STDERR, "Input code failure\n");
                    exit(23);
                }
                fwrite(STDOUT,"\t</instruction>\n");
                break;
            case "STRLEN":
                if(sizeof($exploted)!= 3){
                    fwrite(STDERR, "Input code failure\n");
                    exit(23);
                }
                fwrite(STDOUT,"\t<instruction order=".'"'.$order.'"'." opcode=".'"'.strtoupper($exploted[0]).'"'.">\n");
                $order++;
                if(preg_match("/^(LF|GF|TF)@[a-zA-Z_$\-#&%*!?][a-zA-Z_$\-#&*%!?$0-9]*$/", $exploted[1])){
                    fwrite(STDOUT,"\t\t <arg1 type=\"var\">".$exploted[1]."</arg1>\n");
                }
                else{
                    fwrite(STDERR, "Input code failure\n");
                    exit(23);
                }
                if(substr($exploted[2],0,7)=="string@"){
                    Check_codes(substr($exploted[2],7));
                    fwrite(STDOUT, "\t\t <arg2 type=\"string\">".htmlspecialchars(substr($exploted[2],7))."</arg2>\n");
                }
                else{
                    fwrite(STDERR, "Input code failure\n");
                    exit(23);
                }
                fwrite(STDOUT,"\t</instruction>\n");
                break;
            case "CONCAT":
                if(sizeof($exploted)!= 4){
                    fwrite(STDERR, "Input code failure\n");
                    exit(23);
                }
                fwrite(STDOUT,"\t<instruction order=".'"'.$order.'"'." opcode=".'"'.strtoupper($exploted[0]).'"'.">\n");
                $order++;
                if(preg_match("/^(LF|GF|TF)@[a-zA-Z_$\-#&%*!?][a-zA-Z_$\-#&*%!?$0-9]*$/", $exploted[1])){
                    fwrite(STDOUT,"\t\t <arg1 type=\"var\">".$exploted[1]."</arg1>\n");
                }
                else{
                    fwrite(STDERR, "Input code failure\n");
                    exit(23);
                }
                if(substr($exploted[2],0,7)=="string@"){
                    Check_codes(substr($exploted[2],7));
                    fwrite(STDOUT,"\t\t <arg2 type=\"string\">".htmlspecialchars(substr($exploted[2],7))."</arg2>\n");
                }
                else if(preg_match("/^(LF|GF|TF)@[a-zA-Z_$\-#&%*!?][a-zA-Z_$\-#&*%!?$0-9]*$/", $exploted[2])){
                    fwrite(STDOUT,"\t\t <arg2 type=\"var\">".$exploted[2]."</arg2>\n");
                }
                else{
                    fwrite(STDERR, "Input code failure\n");
                    exit(23);
                }
                if(substr($exploted[3],0,7)=="string@"){
                    Check_codes(substr($exploted[3],7));
                    fwrite(STDOUT,"\t\t <arg3 type=\"string\">".htmlspecialchars(substr($exploted[3],7))."</arg3>\n");
                }
                else if(preg_match("/^(LF|GF|TF)@[a-zA-Z_$\-#&%*!?][a-zA-Z_$\-#&*%!?$0-9]*$/", $exploted[3])){
                    fwrite(STDOUT, "\t\t <arg3 type=\"var\">".$exploted[3]."</arg3>\n");
                }
                else{
                    fwrite(STDERR, "Input code failure\n");
                    exit(23);
                }
                fwrite(STDOUT,"\t</instruction>\n");
                break;
            case "WRITE":
                if(sizeof($exploted)!= 2){
                    fwrite(STDERR, "Input code failure\n");
                    exit(23);
                }
                fwrite(STDOUT, "\t<instruction order=".'"'.$order.'"'." opcode=".'"'.strtoupper($exploted[0]).'"'.">\n");
                $order++;
                
                if(preg_match("/^(LF|GF|TF)@[a-zA-Z_$\-#&*%!?][a-zA-Z_$\-#&%*!?$0-9]*$/", $exploted[1])){
                    fwrite(STDOUT, "\t\t <arg1 type=\"var\">".$exploted[1]."</arg1>\n");
                }
                else if(substr($exploted[1],0,7)=="string@"){
                    Check_codes(substr($exploted[1],7));
                    fwrite(STDOUT, "\t\t <arg1 type=\"string\">".htmlspecialchars(substr($exploted[1],7))."</arg1>\n");
                }
                else if((preg_match("/^int@[0-9][0-9]*$/", $exploted[1])) OR (preg_match("/^int@-[0-9][0-9]*$/", $exploted[1]))){
                    fwrite(STDOUT, "\t\t <arg1 type=\"int\">".substr($exploted[1],4)."</arg1>\n");
                }
                else if(preg_match("/^(bool)@[false|true]/", $exploted[1])){
                    fwrite(STDOUT, "\t\t <arg1 type=\"bool\">".substr($exploted[1],5)."</arg1>\n");
                }
                else if(preg_match("/^(nil)@(nil)$/", $exploted[1])){
                    fwrite(STDOUT, "\t\t <arg1 type=\"nil\">".substr($exploted[1],4)."</arg1>\n");
                }
                else{
                    fwrite(STDERR, "Input code failure\n");
                    exit(23);
                }
                fwrite(STDOUT,"\t</instruction>\n");
                break;
            case "READ":
                if(sizeof($exploted)!= 3){
                    fwrite(STDERR, "Input code failure\n");
                    exit(23);
                }
                fwrite(STDOUT, "\t<instruction order=".'"'.$order.'"'." opcode=".'"'.strtoupper($exploted[0]).'"'.">\n");
                $order++;
                if(preg_match("/^(LF|GF|TF)@[a-zA-Z_$\-#&*%!?][a-zA-Z_$\-#&%*!?$0-9]*$/", $exploted[1])){
                    fwrite(STDOUT, "\t\t <arg1 type=\"var\">".$exploted[1]."</arg1>\n");
                }
                else{
                    fwrite(STDERR, "Input code failure\n");
                    exit(23);
                }
                if($exploted[2] == "string"){
                    fwrite(STDOUT, "\t\t <arg2 type=\"type\">".$exploted[2]."</arg2>\n");
                }
                else if($exploted[2]== "bool"){
                    fwrite(STDOUT, "\t\t <arg2 type=\"type\">".$exploted[2]."</arg2>\n");
                }
                else if($exploted[2] == "int"){
                    fwrite(STDOUT, "\t\t <arg2 type=\"type\">".$exploted[2]."</arg2>\n");
               }
               fwrite(STDOUT,"\t</instruction>\n");
               break;
            case "STR2INT":
                if(sizeof($exploted)!= 4){
                    fwrite(STDERR, "Input code failure\n");
                    exit(23);
                }
                fwrite(STDOUT, "\t<instruction order=".'"'.$order.'"'." opcode=".'"'.strtoupper($exploted[0]).'"'.">\n");
                $order++;
                if(preg_match("/^(LF|GF|TF)@[a-zA-Z_$\-#&*%!?][a-zA-Z_$\-#&%*!?$0-9]*$/", $exploted[1])){
                    fwrite(STDOUT, "\t\t <arg1 type=\"var\">".$exploted[1]."</arg1>\n");
                }
                else{
                    fwrite(STDERR, "Input code failure\n");
                    exit(23);
                }
                if(preg_match("/^(LF|GF|TF)@[a-zA-Z_$\-#&*%!?][a-zA-Z_$\-#&%*!?$0-9]*$/", $exploted[2])){
                    fwrite(STDOUT, "\t\t <arg2 type=\"var\">".$exploted[2]."</arg2>\n");
                }
                else if(substr($exploted[2],0,7)=="string@"){
                    Check_codes(substr($exploted[2],7));
                    fwrite(STDOUT, "\t\t <arg2 type=\"string\">".htmlspecialchars(substr($exploted[2],7))."</arg2>\n");
                }
                else{
                    fwrite(STDERR, "Input code failure\n");
                    exit(23);
                }
                if((preg_match("/^int@[0-9][0-9]*$/", $exploted[3])) OR (preg_match("/^int@-[0-9][0-9]*$/", $exploted[3]))){
                    fwrite(STDOUT, "\t\t <arg3 type=\"int\">".substr($exploted[3],4)."</arg3>\n");
                }
                else if(preg_match("/^(LF|GF|TF)@[a-zA-Z_$\-#&*%!?][a-zA-Z_$\-#&%*!?$0-9]*$/", $exploted[3])){
                    fwrite(STDOUT, "\t\t <arg3 type=\"var\">".$exploted[3]."</arg3>\n");
                }
                else{
                    fwrite(STDERR, "Input code failure\n");
                    exit(23);
                }
                fwrite(STDOUT,"\t</instruction>\n");
                break;
            case "INT2CHAR":
                if(sizeof($exploted)!= 3){
                    fwrite(STDERR, "Input code failure\n");
                    exit(23);
                }
                fwrite(STDOUT, "\t<instruction order=".'"'.$order.'"'." opcode=".'"'.strtoupper($exploted[0]).'"'.">\n");
                $order++;
                if(preg_match("/^(LF|GF|TF)@[a-zA-Z_$\-#&*%!?][a-zA-Z_$\-#&%*!?$0-9]*$/", $exploted[1])){
                    fwrite(STDOUT, "\t\t <arg1 type=\"var\">".$exploted[1]."</arg1>\n");
                }
                else{
                    fwrite(STDERR, "Input code failure\n");
                    exit(23);
                }
                if((preg_match("/^int@[0-9][0-9]*$/", $exploted[2])) OR (preg_match("/^int@-[0-9][0-9]*$/", $exploted[2]))){
                    fwrite(STDOUT, "\t\t <arg3 type=\"int\">".substr($exploted[2],4)."</arg3>\n");
                }
                else if(preg_match("/^(LF|GF|TF)@[a-zA-Z_$\-#&*%!?][a-zA-Z_$\-#&%*!?$0-9]*$/", $exploted[2])){
                    fwrite(STDOUT, "\t\t <arg3 type=\"var\">".$exploted[2]."</arg3>\n");
                }
                else{
                    fwrite(STDERR, "Input code failure\n");
                    exit(23);
                }
                fwrite(STDOUT,"\t</instruction>\n");
                break;
            case "OR":
            case "AND":
                if(sizeof($exploted)!= 4){
                    fwrite(STDERR, "Input code failure\n");
                    exit(23);
                }
                fwrite(STDOUT,"\t<instruction order=".'"'.$order.'"'." opcode=".'"'.strtoupper($exploted[0]).'"'.">\n");
                $order++;

                if(preg_match("/^(LF|GF|TF)@[a-zA-Z_$\-#&*%!?][a-zA-Z_$\-#&%*!?$0-9]*$/", $exploted[1])){
                    fwrite(STDOUT, "\t\t <arg1 type=\"var\">".$exploted[1]."</arg1>\n");
                }
                else{
                    fwrite(STDERR, "Input code failure\n");
                    exit(23);
                }
                if($exploted[2] == "bool@false" OR $exploted[2] == "bool@true"){
                    fwrite(STDOUT, "\t\t <arg2 type=\"bool\">".substr($exploted[2],5)."</arg2>\n");
                }
                else if(preg_match("/^(LF|GF|TF)@[a-zA-Z_$\-#&*%!?][a-zA-Z_$\-#&%*!?$0-9]*$/", $exploted[2])){
                    fwrite(STDOUT, "\t\t <arg2 type=\"var\">".$exploted[2]."</arg2>\n");
                }
                else{
                    fwrite(STDERR, "Input code failure\n");
                    exit(23);
                }
                if($exploted[3] == "bool@false" OR $exploted[3] == "bool@true"){
                    fwrite(STDOUT, "\t\t <arg3 type=\"bool\">".substr($exploted[3],5)."</arg3>\n");
                }
                else if(preg_match("/^(LF|GF|TF)@[a-zA-Z_$\-#&*%!?][a-zA-Z_$\-#&%*!?$0-9]*$/", $exploted[3])){
                    fwrite(STDOUT, "\t\t <arg3 type=\"var\">".$exploted[3]."</arg3>\n");
                }
                else{
                    fwrite(STDERR, "Input code failure\n");
                    exit(23);
                }
                fwrite(STDOUT,"\t</instruction>\n");
                break;
            case "NOT":
                if(sizeof($exploted)!= 3){
                    fwrite(STDERR, "Input code failure\n");
                    exit(23);
                }
                fwrite(STDOUT, "\t<instruction order=".'"'.$order.'"'." opcode=".'"'.strtoupper($exploted[0]).'"'.">\n");
                $order++;

                if(preg_match("/^(LF|GF|TF)@[a-zA-Z_$\-#&*%!?][a-zA-Z_$\-#&%*!?$0-9]*$/", $exploted[1])){
                    fwrite(STDOUT, "\t\t <arg1 type=\"var\">".$exploted[1]."</arg1>\n");
                }
                else{
                    fwrite(STDERR, "Input code failure\n");
                    exit(23);
                }
                if($exploted[2] == "bool@false" OR $exploted[2] == "bool@true"){
                    fwrite(STDOUT, "\t\t <arg2 type=\"bool\">".substr($exploted[2],5)."</arg2>\n");
                }
                else if(preg_match("/^(LF|GF|TF)@[a-zA-Z_$\-#&*%!?][a-zA-Z_$\-#&%*!?$0-9]*$/", $exploted[2])){
                    fwrite(STDOUT, "\t\t <arg2 type=\"var\">".$exploted[2]."</arg2>\n");
                }
                else{
                    fwrite(STDERR, "Input code failure\n");
                    exit(23);
                }
                fwrite(STDOUT,"\t</instruction>\n");
                break;
            case "LT":
            case "GT":
                if(sizeof($exploted)!= 4){
                    fwrite(STDERR, "Input code failure\n");
                    exit(23);
                }
                fwrite(STDOUT, "\t<instruction order=".'"'.$order.'"'." opcode=".'"'.strtoupper($exploted[0]).'"'.">\n");
                $order++;
                if(preg_match("/^(LF|GF|TF)@[a-zA-Z_$\-#&*%!?][a-zA-Z_$\-#&%*!?$0-9]*$/", $exploted[1])){
                    fwrite(STDOUT, "\t\t <arg1 type=\"var\">".$exploted[1]."</arg1>\n");
                }
                else{
                    fwrite(STDERR, "Input code failure\n");
                    exit(23);
                }
                if((preg_match("/^int@[0-9][0-9]*$/", $exploted[2])) OR (preg_match("/^int@-[0-9][0-9]*$/", $exploted[2]))){
                    fwrite(STDOUT, "\t\t <arg2 type=\"int\">".substr($exploted[2],4)."</arg2>\n");
                }
                else if($exploted[2] == "bool@false" OR $exploted[2] == "bool@true"){
                    fwrite(STDOUT, "\t\t <arg2 type=\"bool\">".substr($exploted[2],5)."</arg2>\n");
                }
                else if(preg_match("/^(LF|GF|TF)@[a-zA-Z_$\-#&*%!?][a-zA-Z_$\-#&%*!?$0-9]*$/", $exploted[2])){
                    fwrite(STDOUT, "\t\t <arg2 type=\"var\">".$exploted[2]."</arg2>\n");
                }
                else if(substr($exploted[2],0,7)=="string@"){
                    Check_codes(substr($exploted[2],7));
                    fwrite(STDOUT, "\t\t <arg2 type=\"string\">".htmlspecialchars(substr($exploted[2],7))."</arg2>\n");
                }
                else{
                    fwrite(STDERR, "Input code failure\n");
                    exit(23);
                }
                if((preg_match("/^int@[0-9][0-9]*$/", $exploted[3])) OR (preg_match("/^int@-[0-9][0-9]*$/", $exploted[3]))){
                    fwrite(STDOUT, "\t\t <arg3 type=\"int\">".substr($exploted[3],4)."</arg3>\n");
                }
                else if($exploted[3] == "bool@false" OR $exploted[3] == "bool@true"){
                    fwrite(STDOUT, "\t\t <arg3 type=\"bool\">".substr($exploted[3],5)."</arg3>\n");
                }
                else if(preg_match("/^(LF|GF|TF)@[a-zA-Z_$\-#&*%!?][a-zA-Z_$\-#&%*!?$0-9]*$/", $exploted[3])){
                    fwrite(STDOUT, "\t\t <arg3 type=\"var\">".$exploted[3]."</arg3>\n");
                }
                else if(substr($exploted[3],0,7)=="string@"){
                    Check_codes(substr($exploted[3],7));
                    fwrite(STDOUT, "\t\t <arg3 type=\"string\">".htmlspecialchars(substr($exploted[3],7))."</arg3>\n");
                }
                else{
                    fwrite(STDERR, "Input code failure\n");
                    exit(23);
                }
                fwrite(STDOUT, "\t</instruction>\n");
                break;
            case "EQ":
                if(sizeof($exploted)!= 4){
                    fwrite(STDERR, "Input code failure\n");
                    exit(23);
                }
                fwrite(STDOUT, "\t<instruction order=".'"'.$order.'"'." opcode=".'"'.strtoupper($exploted[0]).'"'.">\n");
                $order++;
                if(preg_match("/^(LF|GF|TF)@[a-zA-Z_$\-#&*%!?][a-zA-Z_$\-#&%*!?$0-9]*$/", $exploted[1])){
                    fwrite(STDOUT, "\t\t <arg1 type=\"var\">".$exploted[1]."</arg1>\n");
                }
                else{
                    fwrite(STDERR, "Input code failure\n");
                    exit(23);
                }
                if((preg_match("/^int@[0-9][0-9]*$/", $exploted[2])) OR (preg_match("/^int@-[0-9][0-9]*$/", $exploted[2]))){
                    fwrite(STDOUT, "\t\t <arg2 type=\"int\">".substr($exploted[2],4)."</arg2>\n");
                }
                else if($exploted[2] == "bool@false" OR $exploted[2] == "bool@true"){
                    fwrite(STDOUT, "\t\t <arg2 type=\"bool\">".substr($exploted[2],5)."</arg2>\n");
                }
                else if(preg_match("/^(LF|GF|TF)@[a-zA-Z_$\-#&*%!?][a-zA-Z_$\-#&%*!?$0-9]*$/", $exploted[2])){
                    fwrite(STDOUT, "\t\t <arg2 type=\"var\">".$exploted[2]."</arg2>\n");
                }
                else if(substr($exploted[2],0,7)=="string@"){
                    Check_codes(substr($exploted[2],7));
                    fwrite(STDOUT, "\t\t <arg2 type=\"string\">".htmlspecialchars(substr($exploted[2],7))."</arg2>\n");
                }
                else if($exploted[2]=="nil@nil"){
                    fwrite(STDOUT, "\t\t <arg2 type=\"nil\">"."nil"."</arg2>\n");
                }
                else{
                    fwrite(STDERR, "Input code failure\n");
                    exit(23);
                }
                if((preg_match("/^int@[0-9][0-9]*$/", $exploted[3])) OR (preg_match("/^int@-[0-9][0-9]*$/", $exploted[3]))){
                    fwrite(STDOUT, "\t\t <arg3 type=\"int\">".substr($exploted[3],4)."</arg3>\n");
                }
                else if($exploted[3] == "bool@false" OR $exploted[3] == "bool@true"){
                    fwrite(STDOUT, "\t\t <arg3 type=\"bool\">".substr($exploted[3],5)."</arg3>\n");
                }
                else if(preg_match("/^(LF|GF|TF)@[a-zA-Z_$\-#&*%!?][a-zA-Z_$\-#&%*!?$0-9]*$/", $exploted[3])){
                    fwrite(STDOUT, "\t\t <arg3 type=\"var\">".$exploted[3]."</arg3>\n");
                }
                else if(substr($exploted[3],0,7)=="string@"){
                    Check_codes(substr($exploted[3],7));
                    fwrite(STDOUT, "\t\t <arg3 type=\"string\">".htmlspecialchars(substr($exploted[3],7))."</arg3>\n");
                }
                else if($exploted[3]=="nil@nil"){
                    fwrite(STDOUT, "\t\t <arg2 type=\"nil\">"."nil"."</arg2>\n");
                }
                else{
                    fwrite(STDERR, "Input code failure\n");
                    exit(23);
                }
                fwrite(STDOUT,"\t</instruction>\n");
                break;
            case "ADD":
            case "SUB":
            case "MUL":
            case "IDIV":
                if(sizeof($exploted)!= 4){
                    fwrite(STDERR, "Input code failure\n");
                    exit(23);
                }
                fwrite(STDOUT, "\t<instruction order=".'"'.$order.'"'." opcode=".'"'.strtoupper($exploted[0]).'"'.">\n");
                $order++;
                if(preg_match("/^(LF|GF|TF)@[a-zA-Z_$\-#&*%!?][a-zA-Z_$\-#&%*!?$0-9]*$/", $exploted[1])){
                    fwrite(STDOUT, "\t\t <arg1 type=\"var\">".$exploted[1]."</arg1>\n");
                }
                else{
                    fwrite(STDERR, "Input code failure\n");
                    exit(23);
                }
                if((preg_match("/^int@[0-9][0-9]*$/", $exploted[2])) OR (preg_match("/^int@-[0-9][0-9]*$/", $exploted[2]))){
                    fwrite(STDOUT, "\t\t <arg2 type=\"int\">".substr($exploted[2],4)."</arg2>\n");
                }
                else if(preg_match("/^(LF|GF|TF)@[a-zA-Z_$\-#&*%!?][a-zA-Z_$\-#&%*!?$0-9]*$/", $exploted[2])){
                    fwrite(STDOUT, "\t\t <arg2 type=\"var\">".$exploted[2]."</arg2>\n");
                }
                else{
                    fwrite(STDERR, "Input code failure\n");
                    exit(23);
                }
                if((preg_match("/^int@[0-9][0-9]*$/", $exploted[3])) OR (preg_match("/^int@-[0-9][0-9]*$/", $exploted[3]))){
                    fwrite(STDOUT, "\t\t <arg3 type=\"int\">".substr($exploted[3],4)."</arg3>\n");
                }
                else if(preg_match("/^(LF|GF|TF)@[a-zA-Z_$\-#&*%!?][a-zA-Z_$\-#&%*!?$0-9]*$/", $exploted[3])){
                    fwrite(STDOUT, "\t\t <arg3 type=\"var\">".$exploted[3]."</arg3>\n");
                }
                else{
                    fwrite(STDERR, "Input code failure\n");
                    exit(23);
                }
                fwrite(STDOUT,"\t</instruction>\n");
                break;
            case "POPS":
                if(sizeof($exploted)!= 2){
                    fwrite(STDERR, "Input code failure\n");
                    exit(23);
                }
                fwrite(STDOUT, "\t<instruction order=".'"'.$order.'"'." opcode=".'"'.strtoupper($exploted[0]).'"'.">\n");
                $order++;
                if(preg_match("/^(LF|GF|TF)@[a-zA-Z_$\-&%*!?][a-zA-Z_$\-&*%!?$0-9]*$/", $exploted[1])){
                    fwrite(STDOUT, "\t\t <arg1 type=\"var\">".$exploted[1]."</arg1>\n");
                    fwrite(STDOUT,"\t</instruction>\n");
                    break;
                }
                else{
                    fwrite(STDERR, "Input code failure\n");
                    exit(23);
                }
            case "PUSHS":
                if(sizeof($exploted)!= 2){
                    fwrite(STDERR, "Input code failure\n");
                    exit(23);
                }
                fwrite(STDOUT, "\t<instruction order=".'"'.$order.'"'." opcode=".'"'.strtoupper($exploted[0]).'"'.">\n");
                $order++;
                if(preg_match("/^(LF|GF|TF)@[a-zA-Z_$\-#&*%!?][a-zA-Z_$\-#&%*!?$0-9]*$/", $exploted[1])){
                    fwrite(STDOUT, "\t\t <arg1 type=\"var\">".$exploted[1]."</arg1>\n");
                }
                else if(substr($exploted[1],0,7)=="string@"){
                    Check_codes(substr($exploted[1],7));
                    fwrite(STDOUT, "\t\t <arg1 type=\"string\">".htmlspecialchars(substr($exploted[1],7))."</arg1>\n");
                }
                else if((preg_match("/^int@[0-9][0-9]*$/", $exploted[1])) OR (preg_match("/^int@-[0-9][0-9]*$/", $exploted[1]))){
                    fwrite(STDOUT, "\t\t <arg1 type=\"int\">".substr($exploted[1],4)."</arg1>\n");
                }
                else if(preg_match("/^(bool)@[false|true]/", $exploted[1])){
                    fwrite(STDOUT, "\t\t <arg1 type=\"bool\">".substr($exploted[1],5)."</arg1>\n");
                }
                else if(preg_match("/^(nil)@(nil)$/", $exploted[1])){
                    fwrite(STDOUT, "\t\t <arg1 type=\"nil\">".substr($exploted[1],4)."</arg1>\n");
                }
                else{
                    fwrite(STDERR, "Input code failure\n");
                    exit(23);
                }
                fwrite(STDOUT,"\t</instruction>\n");
                break;
            case "RETURN":
                if(sizeof($exploted)!= 1){
                    fwrite(STDERR, "Input code failure\n");
                    exit(23);
                }
                fwrite(STDOUT, "\t<instruction order=".'"'.$order.'"'." opcode=".'"'.strtoupper($exploted[0]).'"'."/>\n");
                $order++;
                break;
            case "CALL":
                if(sizeof($exploted)!= 2){
                    fwrite(STDERR, "Input code failure\n");
                    exit(23);
                }
                fwrite(STDOUT, "\t<instruction order=".'"'.$order.'"'." opcode=".'"'.strtoupper($exploted[0]).'"'.">\n");
                $order++;
                if(is_numeric($exploted[1][0])){
                    fwrite(STDERR, "Input code failure\n");
                    exit(23);
                }
                if(preg_match("/^[a-zA-Z_$\-&*%!?$0-9]*$/", $exploted[1])){
                    fwrite(STDOUT, "\t\t <arg1 type=\"label\">".$exploted[1]."</arg1>\n");
                    fwrite(STDOUT, "\t</instruction>\n");
                    break;
                }
                else{
                    fwrite(STDERR, "Input code failure\n");
                    exit(23);
                }
                fwrite(STDOUT,"\t</instruction>\n");
                break;
            case "CREATEFRAME":
            case "POPFRAME":
            case "PUSHFRAME":
                if(sizeof($exploted)!= 1){
                    fwrite(STDERR, "Input code failure\n");
                    exit(23);
                }
                fwrite(STDOUT,"\t<instruction order=".'"'.$order.'"'." opcode=".'"'.strtoupper($exploted[0]).'"'."/>\n");
                $order++;
                break;
            case "MOVE":
                if(sizeof($exploted)!= 3){
                    fwrite(STDERR, "Input code failure\n");
                    exit(23);
                }
                fwrite(STDOUT, "\t<instruction order=".'"'.$order.'"'." opcode=".'"'.strtoupper($exploted[0]).'"'.">\n");
                $order++;
                if(preg_match("/^(LF|GF|TF)@[a-zA-Z_$\-#&*%!?][a-zA-Z_$\-#&%*!?$0-9]*$/", $exploted[1])){
                    fwrite(STDOUT, "\t\t <arg1 type=\"var\">".$exploted[1]."</arg1>\n");
                }
                else{
                    fwrite(STDERR, "Input code failure\n");
                    exit(23);
                }
                if(preg_match("/^(LF|GF|TF)@[a-zA-Z_$\-#&*%!?][a-zA-Z_$\-#&%*!?$0-9]*$/", $exploted[2])){
                    fwrite(STDOUT, "\t\t <arg1 type=\"var\">".$exploted[2]."</arg1>\n");
                }
                else if(substr($exploted[2],0,7)=="string@"){
                    Check_codes(substr($exploted[2],7));
                    fwrite(STDOUT, "\t\t <arg2 type=\"string\">".htmlspecialchars(substr($exploted[2],7))."</arg2>\n");
                }
                else if((preg_match("/^int@[0-9][0-9]*$/", $exploted[2])) OR (preg_match("/^int@-[0-9][0-9]*$/", $exploted[2]))){
                    fwrite(STDOUT, "\t\t <arg2 type=\"int\">".substr($exploted[2],4)."</arg2>\n");
                }
                else if(preg_match("/^(bool)@[false|true]/", $exploted[2])){
                    fwrite(STDOUT, "\t\t <arg2 type=\"bool\">".substr($exploted[2],5)."</arg2>\n");
                }
                else if(preg_match("/^(nil)@(nil)$/", $exploted[2])){
                    fwrite(STDOUT, "\t\t <arg2 type=\"nil\">".substr($exploted[2],4)."</arg2>\n");
                }
                else{
                    fwrite(STDERR, "Input code failure\n");
                    exit(23);
                }
                fwrite(STDOUT,"\t</instruction>\n");
                break;
            case "\n":
                break;
            default:
                if(substr($exploted[0],0,1 != "#")){
                    fwrite(STDERR,"Unknown code failure\n");
                    exit(22);
                }
        }
    }
    fwrite(STDOUT,"</program>\n");  
    exit (0);
?>