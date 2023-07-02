/*
* Program laczacy sie ze strona https://www.accuweather.com, pobierajacy dane dotyczace aktualnego
* poziomu powietrza i wypisujacy je na standardowe wyjscie
* Porgram moze przyjac trzy argumenty: nazwe miasta, nazwe miasta w linku, id miasta w linku
* przykladowo dla miasta Warszawy: "Warszawa", "warsaw", "274663"
* W przypadku braku argumentow program wyszuka informacje dla miasta Krakow.
* */
package org.example;

import org.jsoup.Connection;
import org.jsoup.Jsoup;
import org.jsoup.nodes.Document;
import org.jsoup.nodes.Element;
import org.jsoup.select.Elements;
import java.io.IOException;
import java.util.Arrays;


public class WebScrapper {

    //nice colors
    public static final String ANSI_RED = "\u001B[31m";
    public static final String ANSI_GREEN = "\u001B[32m";
    public static final String ANSI_RESET = "\u001B[0m";

    private static Connection connectToWebsite(String url, String webType, String webContent) {

        Document doc = null;
        Connection connection = Jsoup.connect(url);
        try {
            doc = connection.get();
        } catch (IOException e) {
            e.printStackTrace();
            System.exit(2); //failure status
        }
        Connection.Response response = connection.response();

        //check the code
        if(response.statusCode() != 200) {
            System.exit(3);
        }
        //check type
        String responseType = response.contentType();
        // sometimes returning value of .contentType() is text/html; charset=utf-8
        int ix;
        assert responseType != null;
        if((ix = responseType.indexOf(';')) != -1){
            responseType = responseType.substring(0, ix);
        }
        if(!webType.equals(responseType)) {
            System.exit(4);
        }

        //check if concrete content on the website exist
        Elements docElementsContainingText = doc.getElementsContainingText(webContent);
        if(docElementsContainingText.isEmpty()) {
            System.exit(5);
        }
        return connection;
    }

    private static boolean isCorrectAirQualityNumber(String nums) {
        int aqNumber;
        try {
            aqNumber = Integer.parseInt(nums);
        } catch (NumberFormatException e) {
            return false;
        }

        return aqNumber >= 0 && aqNumber <= 500; //biggest expected value is more than 301
    }

    private static int getAirQualityNumber(Connection connection){
        Document doc = null;
        try {
            doc = connection.get();
        } catch (IOException e) {
            e.printStackTrace();
            System.exit(6); //failure status
        }

        Element airQualityContent = doc.select("div.air-quality-content").first();
        if(airQualityContent == null){
            System.exit(7);
        }

        String nums = null;

        if(!airQualityContent.select("p.day-of-week").text().equals("Dzisiaj")){
            System.exit(8);
        }

         nums = airQualityContent.select("div.aq-number").text();

        //sprawdz czy pobrane wartosci to wartosc zanieczyszczenia i jednostka
        if(!isCorrectAirQualityNumber(nums)){
            System.exit(9);
        }
        String Aqunit = airQualityContent.select("div.aq-unit").text();
        if(!Aqunit.equals("AQI")){
            System.exit(10);
        }

        return Integer.parseInt(nums);
    }

    public static void main(String[] args) {

        String[] cityData;

        if (args.length == 3) {
            cityData = Arrays.copyOf(args, 3);
        } else {

            cityData = new String[]{"Kraków", "krakow", "274455"};
            //cityData = new String[]{"Warszawa", "warsaw", "274663"};
        }

        String url, webType, webContent;
        url = "https://www.accuweather.com/pl/pl/" + cityData[1] + "/"
                + cityData[2] + "/air-quality-index/" + cityData[2];

        webType = "text/html";
        webContent = "INDEKS"; //powinno znajdować sie na stronie

        System.err.print("Connecting to " + url + ": ");
        Connection connection = connectToWebsite(url, webType, webContent);
        System.err.println(ANSI_GREEN + "OK" + ANSI_RESET);
        int aqNumber = getAirQualityNumber(connection);
        System.err.println("Aktualne zanieczyszczenie w mieście " + cityData[0]
                + ": " + aqNumber + " AQI.");

        //wypisz na standatdowe wyjscie tylko sama wartosc zanieczyszczenia
        System.out.println(aqNumber);

        System.exit(0); //success status
    }
}
