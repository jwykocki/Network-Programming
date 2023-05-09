package org.example;

import org.jsoup.Connection;
import org.jsoup.Jsoup;
import org.jsoup.nodes.Document;
import org.jsoup.nodes.Element;
import org.jsoup.select.Elements;

import java.io.IOException;

public class WebScrapper {

    //colors
    public static final String ANSI_RED = "\u001B[31m";
    public static final String ANSI_GREEN = "\u001B[32m";
    public static final String ANSI_RESET = "\u001B[0m";

    //TODO: lepiej robic z JSONem
    public static void main(String[] args) {

        int webCode = 200;
        City city;

        if(args.length==3){
            city = new City(args[0], args[1], args[2]);
        }else {
            city = new City("Kraków", "krakow", "274455");
        }

        String url, webType, webContent;

        url = "https://www.accuweather.com/pl/pl/" + city.getURLname() +"/"
                + city.getId() + "/air-quality-index/" +  city.getId();
        webType = "text/html";
        webContent = "AKTUALNA";


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
        Elements docElementsContainingText = doc.getElementsContainingText(webContent);

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
        if(!docElementsContainingText.isEmpty()){
            System.out.println(ANSI_GREEN + "\t[OK]" + ANSI_RESET);
        }else {
            System.out.println(ANSI_RED + "\t[NOT OK]" + ANSI_RESET);
            System.exit(4);
        }

        Element airQualityContent = doc.select("div.air-quality-content").first();
        if(airQualityContent == null){
            System.exit(5);
        }

        //TODO: sprawdz czy te wartosci maja sens
        if(airQualityContent.select("p.day-of-week").text().equals("Dzisiaj")){
            int AqNumber = Integer.parseInt(airQualityContent.select("div.aq-number").text());
            String Aqunit = airQualityContent.select("div.aq-unit").text();
            System.out.println("Aktualne zanieczyszczenie w mieście " + city.getName()
                    + ": " + AqNumber + " " + Aqunit + ".");
        }else {
            System.exit(6);
        }

        System.exit(0); //success status

    }
}
