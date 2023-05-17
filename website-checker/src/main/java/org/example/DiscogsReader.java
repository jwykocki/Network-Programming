package org.example;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;
import java.net.URI;
import java.net.http.HttpClient;
import java.net.http.HttpRequest;
import java.net.http.HttpResponse;

public class DiscogsReader {
    private static final HttpClient httpClient = HttpClient.newBuilder()
            .version(HttpClient.Version.HTTP_2)
            .build();

    public static final String ANSI_GREEN = "\u001B[32m";
    public static final String ANSI_RESET = "\u001B[0m";

    private static JSONObject parseJSON(String response){

        JSONObject jobj = null;
        try {
            jobj = new JSONObject(response);
        }catch(JSONException e){
            e.printStackTrace();
            System.exit(4);
        }
        return jobj;
    }

    private static JSONArray getReleases(JSONObject artist){

        JSONObject data;
        JSONArray releases = null;

        try{
            data = (JSONObject) artist.get("data");
            releases = data.getJSONArray("releases");
        }catch(JSONException e){
            e.printStackTrace();
            System.exit(11);
        }

        if(releases == null){
            System.exit(12);
        }

        return releases;
    }


    private static String sendGet(String url) {
        HttpResponse<String> response = null;
        HttpRequest request = HttpRequest.newBuilder()
                .GET()
                .uri(URI.create(url))
                .setHeader("User-Agent", "Java 11 HttpClient Bot")
                .build();

        try {
            response = httpClient.send(request, HttpResponse.BodyHandlers.ofString());
        }catch(java.io.IOException | java.lang.InterruptedException e){
            e.printStackTrace();
            System.exit(14);
        }

        if(response.statusCode()!=200){
            System.err.println("Status code: " + response.statusCode());
            System.exit(15);
        }

        String resp = response.body();
        try {
            resp = resp.substring(resp.indexOf('{')); //usun callbackname na poczatku
        }catch(IndexOutOfBoundsException e){
            e.printStackTrace();
            System.exit(16);
        }

        return resp;
    }

    private static String getArtistName(JSONObject json){
        JSONArray releases = getReleases(json);
        String name = null;
        for(int i=0; i<releases.length(); i++){
            JSONObject release = (JSONObject) releases.get(i);
             if(release.get("role").equals("Main")) { //szukamy solowych albumow
                name = release.get("artist").toString();
             }
        }

        if(name == null){
            System.err.println("Artist name not found");
            System.exit(20);
        }
        return name;
    }


    public static void main(String[] args) throws Exception {

        String url = "https://api.discogs.com/artists/4320863/releases?callback=callbackname";
        System.err.print("Connecting to " + url + ": ");

        String response = sendGet(url);
        System.err.println(ANSI_GREEN + "OK" + ANSI_RESET);

        JSONObject json = parseJSON(response);
        JSONArray releases = getReleases(json);
        System.out.println(getArtistName(json));

        //wyswietl albumy na stdout
        for(int i=0; i<releases.length(); i++){
            JSONObject release = (JSONObject) releases.get(i);
           if(release.get("role").equals("Main")) { //szukamy solowych albumow
                System.out.println(release.get("title"));
           }
        }
    }
}
