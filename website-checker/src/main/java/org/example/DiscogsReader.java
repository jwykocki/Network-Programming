package org.example;

import org.json.JSONArray;
import org.json.JSONObject;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.net.MalformedURLException;
import java.net.URI;
import java.net.URL;

public class DiscogsReader {

    private static JSONObject getJSONFromWebsite(String url){
        URL obj = null;
        StringBuilder jsonStringBuilder;
        try {
            URI uri = new URI(url);
            obj = uri.toURL();
        } catch (MalformedURLException e) {
            e.printStackTrace();
            System.exit(2);

        } catch (java.net.URISyntaxException e) {
            e.printStackTrace();
            System.exit(7);
        }

        try (InputStream input = obj.openStream()) {
            InputStreamReader isr = new InputStreamReader(input);
            BufferedReader reader = new BufferedReader(isr);
            jsonStringBuilder = new StringBuilder();
            int c;
            while ((c = reader.read()) != -1) {
                jsonStringBuilder.append((char) c);
            }

        } catch (IOException e) {
            throw new RuntimeException(e);
        }
        String jsonWholeString = jsonStringBuilder.toString();
        return  new JSONObject(jsonWholeString.substring(jsonWholeString.indexOf('{'), jsonWholeString.length()-1));
    }

    private static boolean checkConnection(JSONObject json){
        //check status code
        JSONObject meta = (JSONObject) json.get("meta");

        int statusCode = (int) meta.get("status");
        if(statusCode!=200) System.exit(10);

        return true;
    }

    public static void main(String[] args) {

        String url = "https://api.discogs.com/artists/4320863/releases?callback=callbackname";
        JSONObject json = getJSONFromWebsite(url);

        JSONObject data = (JSONObject) json.get("data");
        JSONArray releases = data.getJSONArray("releases");

        for(int i=0; i< releases.length(); i++){
            JSONObject release = (JSONObject) releases.get(i);
            if(release.get("role").equals("Main")) { //szukamy solowych albumow
                System.out.println(release.get("title"));
            }
        }
    }
}
