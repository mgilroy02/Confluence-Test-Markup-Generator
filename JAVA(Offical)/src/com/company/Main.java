package com.company;

import java.io.*;
import java.net.URISyntaxException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.Arrays;
import java.util.Optional;

/**
 * Hey Critic,  This code is so hacky but it was wrote in 30 minutes while watching TV
 * Goal was to be faster to do this than to document tests I wrote for a ticket
 */
public class Main {

    enum keywords {
        DEF("def \""), GIVEN("given:"), WHEN("when:"), THEN("then:"), AND("and:"), WHERE("where:");


        public final String literal;
        public String getLiteral(){return this.literal;}

        private keywords(String literal){
            this.literal = literal;
        }
    }

    public static void main(String[] args) throws URISyntaxException {
        String txtFileName = "";
        if (args.length > 1){
            System.out.println("First and only arg is the file name that contains the tests");
            return;
        } else if (args.length == 0){
            txtFileName = "sample.txt";
        } else {
            txtFileName = args[0];
        }

        //String directory = new File(Main.class.getProtectionDomain().getCodeSource().getLocation().toURI()).getPath().replace("TestMarkupGenerator.jar", "") + "\\" + txtFileName;
        String directory = new File(Main.class.getProtectionDomain().getCodeSource().getLocation().toURI()).getPath().replace("TestMarkupGenerator.jar", "") + "/" + txtFileName;
        System.out.println("Working with : " + directory);

        Path path = Paths.get(directory);
        if (!Files.exists(path)){
            System.out.println("File Doesn't exist" + path);
            return;
        }

        try {
            readFile(directory);
        } catch (IOException ex){
            System.out.println("Something went wrong printing the file");
            System.out.println(ex.getStackTrace());
        }
    }

    public static void readFile(String directory) throws IOException {
        BufferedReader reader = new BufferedReader(new FileReader(directory));

        String line;
        boolean inWhere = false;
        boolean firstTest = true;
        while ((line = reader.readLine()) != null){

            String finalLine = line;
            Optional<keywords> keywordFound = Arrays.asList(keywords.values()).stream().filter(e -> finalLine.contains(e.getLiteral())).findFirst();

            if (inWhere){
                if (line.trim().isEmpty() || line.trim().equals("}")){
                    inWhere = false;
                    print("{expand}"); //Close off the expand box
                    continue;
                }
                print("**" + line); //Tabs & bulletpoints the string
            }
            if (!keywordFound.isPresent()){
                continue;
            }

            line =  line.replace(keywordFound.get().getLiteral(), ""); //Replace "Given: We run a test" with "", so then we just have "We run a test"
            keywords keyWord = keywordFound.get();

            line = line.trim();
            line = line.replace("\"", "");
            line = line.replace("and:", ""); //Could add an if statement for AND but performance wise and spaghetti wise, just have a replace on "and:"
            if (keyWord == keyWord.DEF){
                if (!firstTest){
                    print("{expand}"); //Close off the expand box
                } else {
                    firstTest = false;
                }
                print("\n\n\n");
                line = line.substring(0, line.indexOf("("));
                print("{expand:title=" + line + "}"); //print expand box
                print("h3." + line);
            } else {
                if (keyWord == keyWord.WHERE){
                    inWhere = true;
                }
                print("\n");
                if (keyWord != keyWord.AND){
                    print("*" + keyWord.getLiteral() + "*"); //Prints and Bolds Keyword
                }
                print("* " + line); //Tabs & bulletpoints the string
            }
        }


        reader.close();
    }

    public static void print(String line){
        System.out.println(line);
    }


}
