/*
* Program laczacy sie ze strona https://www.accuweather.com, pobierajacy dane dotyczace aktualnego
* poziomu powietrza i wypisujacy je na standardowe wyjscie
* Porgram moze przyjac trzy argumenty: nazwe miasta, nazwe miasta w linku, id miasta w linku
* przykladowo dla miasta Warszawy: "Warszawa", "warsaw", "274663"
* W przypadku braku argumentow program wyszuka informacje dla miasta Krakow.
* */

package org.example;

import netscape.javascript.JSObject;
//import org.json.JSONObject;
import org.json.*;
import org.jsoup.Connection;
import org.jsoup.Jsoup;
import org.jsoup.nodes.Document;
import org.jsoup.nodes.Element;
import org.jsoup.select.Elements;
import org.springframework.web.reactive.function.client.WebClient;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.HttpURLConnection;
import java.net.MalformedURLException;
import java.net.URL;
import java.util.Arrays;
import java.util.Iterator;


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
            System.exit(2); //failure status
        }

        Element airQualityContent = doc.select("div.air-quality-content").first();
        if(airQualityContent == null){
            System.exit(5);
        }

        String nums = null;

        if(airQualityContent.select("p.day-of-week").text().equals("Dzisiaj")){
             nums = airQualityContent.select("div.aq-number").text();

            //sprawdz czy pobrane wartosci to wartosc zanieczyszczenia i jednostka
            if(!isCorrectAirQualityNumber(nums)){
                System.exit(7);
            }
            String Aqunit = airQualityContent.select("div.aq-unit").text();
            if(!Aqunit.equals("AQI")){
                System.exit(8);
            }
        }else {
            System.exit(6);
        }
        return Integer.parseInt(nums);
    }

    private static void printWebsite(Connection connection){
        Document doc = null;
        try {
            doc = connection.get();
        } catch (IOException e) {
            e.printStackTrace();
            System.exit(2); //failure status
        }

        System.out.println(doc);
    }

    public static void main(String[] args) throws IOException {

        City city;

        if (args.length == 3) {
            city = new City(args[0], args[1], args[2]);
        } else {
            city = new City("Kraków", "krakow", "274455");
        }

        String url, webType, webContent;
        url = "https://www.accuweather.com/pl/pl/" + city.getURLname() + "/"
                + city.getId() + "/air-quality-index/" + city.getId();
        webType = "text/html";
        webContent = "AKTUALNA"; //powinno znajdować sie na stronie

        String url2 = "https://www.ryanair.com/pl/pl/tanie-loty/?from=KRK&out-from-date=2023-05-11&out-to-date=2024-05-11&budget=150";

        String webContent2 = "Olsztyn";

        System.out.print("Connecting to " + url2 + ": ");
        Connection connection = connectToWebsite(url2, webType, webContent2);
//        System.out.println(ANSI_GREEN + "OK" + ANSI_RESET);
//        int aqNumber = getAirQualityNumber(connection);
//        System.out.println("Aktualne zanieczyszczenie w mieście " + city.getName()
//                + ": " + aqNumber + " AQI.");

        printWebsite(connection);


        System.exit(0); //success status

    }
}
