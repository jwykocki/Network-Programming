package web.scrappers;

import org.json.JSONObject;
import org.json.JSONTokener;
import org.jsoup.select.Elements;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.HttpURLConnection;
import java.net.MalformedURLException;
import java.net.URI;
import java.net.URL;
import java.sql.Connection;

public class JsonParser {

    private static HttpURLConnection connectToWebsite(String URL, String webType, String webContent){
        URL obj = null;
        HttpURLConnection con = null;
        try {
            URI uri = new URI(URL);
            obj = uri.toURL();
        }catch(MalformedURLException e){
            e.printStackTrace();
            System.exit(2);

        }catch(java.net.URISyntaxException e){
            e.printStackTrace();
            System.exit(7);
        }
        try {
            con = (HttpURLConnection) obj.openConnection();
            con.connect();
        }catch(IOException e){
            e.printStackTrace();
            System.exit(3);
        }

        int code = 0;

        //check the code
        try {
            code = con.getResponseCode();
        } catch (IOException e) {
            System.exit(4);
        }

        if(code != 200) {
            System.exit(3);
        }

        //check type
        String conType = con.getContentType();
        System.out.println(conType);
        // sometimes returning value of .contentType() is text/html; charset=utf-8
        int ix;
        assert conType != null;
        if((ix = conType.indexOf(';')) != -1){
            conType = conType.substring(0, ix);
        }
        if(!webType.equals(conType)) {
            System.exit(4);
        }

        //System.out.println(con.);
        /*

        //check if concrete content on the website exist
        Elements docElementsContainingText = doc.getElementsContainingText(webContent);
        if(docElementsContainingText.isEmpty()) {
            System.exit(5);
        }*/
        return con;
    }
    public static void main(String[] args) {

        BufferedReader in = null;

        HttpURLConnection con = connectToWebsite("https://www.ryanair.com/pl/pl/tanie-loty/?from=KRK&out-from-date=2023-05-11&out-to-date=2024-05-11&budget=150", "text/html", "");
        try {
            in = new BufferedReader(new InputStreamReader(con.getInputStream()));
        }catch (IOException e){
            e.printStackTrace();
            System.exit(6);
        }
        String inputLine;
        StringBuilder response = new StringBuilder();

        try {
            while ((inputLine = in.readLine()) != null) {
                response.append(inputLine);
            }
        }catch (IOException e){
            e.printStackTrace();
            System.exit(7);
        }

//        String text = response.toString();
//        int lawReportIx = text.indexOf("oLawReport");
//        String halfText = text.substring(lawReportIx);
//        String jsonText = halfText.substring(halfText.indexOf('{'), halfText.indexOf(';'));
//        JSONObject json = new JSONObject(new JSONTokener(jsonText));
//        JSONObject mst = new JSONObject(new JSONTokener(json.get("mst").toString()));
//        System.out.println(mst.get("desc1").toString());
            String text = response.toString();
        System.out.println(text.contains("Olsztyn"));
    }
}
