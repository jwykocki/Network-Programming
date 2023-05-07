package org.example;

import org.jsoup.Connection;
import org.jsoup.Jsoup;
import org.jsoup.nodes.Document;
import org.jsoup.select.Elements;

import java.io.IOException;

public class WebChecker {

    //colors
    public static final String ANSI_RED = "\u001B[31m";
    public static final String ANSI_GREEN = "\u001B[32m";
    public static final String ANSI_RESET = "\u001B[0m";


    public static void main(String[] args) {

        int webCode = 200;

        String url = args[0]; //"http://th.if.uj.edu.pl/";
        String webType = args[1];//"text/html";
        String webContent = args[2]; //= "Jagiellonian University in Krakow";

        Document doc = null;
        System.out.println("Connecting to " + url);
        Connection connect = Jsoup.connect(url);

        try{
            doc = connect.get();
        }
        catch(IOException e ){
            e.printStackTrace();
            System.exit(1); //failure status
        }

        Connection.Response response = connect.response();

        //get the code
        int responseCode = response.statusCode();

        //get type
        String responseType = response.contentType();
        // sometimes returning value of .contentType() is text/html; charset=utf-8
        int ix;
        assert responseType != null;
        if((ix = responseType.indexOf(';')) != -1){
            responseType = responseType.substring(0, ix);
        }

        //check if concrete content on the website exist
        Elements elements = doc.getElementsContainingText(webContent);

        System.out.println("code = " + responseCode);
        System.out.println("type = " + responseType);
        System.out.println("searching for content: " + webContent);

        System.out.print("webCode: ");
        if(responseCode == webCode){
            System.out.println(ANSI_GREEN + "\t\t[OK]" + ANSI_RESET);
        }else {
            System.out.println(ANSI_RED + "\t\t[NOT OK]" + ANSI_RESET);
            System.exit(2);
        }

        System.out.print("webType: ");
        if(webType.equals(responseType)){
            System.out.println(ANSI_GREEN + "\t\t[OK]" + ANSI_RESET);
        }else {
            System.out.println(ANSI_RED + "\t\t[NOT OK]" + ANSI_RESET);
            System.exit(3);
        }

        System.out.print("webContent: ");
        if(!elements.isEmpty()){
            System.out.println(ANSI_GREEN + "\t[OK]" + ANSI_RESET);
        }else {
            System.out.println(ANSI_RED + "\t[NOT OK]" + ANSI_RESET);
            System.exit(4);
        }

        System.exit(0); //success status

    }
}
