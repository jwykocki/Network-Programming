package org.example;
/*
* Program, który w oparciu o informacje z Discogs sprawdza, czy członkowie (aktualni bądź byli) danego zespołu grali razem
*  w jakichś innych zespołach. Wyświetla nazwy tych innych zespołów oraz nazwiska muzyków. Program przyjmuje jako argument
*  numeryczny identyfikator zespołu (wtedy wyszukiwanie nastepuje od razu) lub nazwe zespolu (wtedy najpierw ta nazwa jest
* wyszukiwana na stronie Discogs, pobierany jest pierwszy wynik z otrzymanych, oraz stamtad wyciagne
* jest ID zespolu. W celu umozliwienia wyszukiwania zalozone zostalo konto na stronie Discogs, uzyskany w ten sposob token zapisany
* jest w zmiennej w main(). Program zapisuje otrzymane wyniki do formatu JSON, oraz zapisuje w pliku output.json.
* Program w trakcie dzialania wypisuje pomocnicze komunikaty na wyjscie System.err, na System.out wypisywane sa tylko koncowe
* wyniki w formacie JSON.
* */
import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.net.URI;
import java.net.http.HttpClient;
import java.net.http.HttpRequest;
import java.net.http.HttpResponse;
import java.util.*;


public class DiscogsReader {
    private static final HttpClient httpClient = HttpClient.newBuilder()
            .version(HttpClient.Version.HTTP_2)
            .build();

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

        //sprawdzic czy poprawny json

        return resp;
    }

    private static class Group{
        private final String name;
        private final ArrayList<String> members;

        public Group(String name) {
            this.name = name;
            members = new ArrayList<>();
        }

        public String getName() {
            return name;
        }

        public ArrayList<String> getMembers() {
            return members;
        }

        public void addMember(String artist){
            if(!members.contains(artist)){
                members.add(artist);
            }
        }
    }

    private static JSONArray getItemsFromData(JSONObject object, String key){
        JSONObject data;
        JSONArray items = null;

        try{
            data = (JSONObject) object.get("data");
            items = data.getJSONArray(key);
        }catch(JSONException e){
            e.printStackTrace();
            System.exit(11);
        }

        if(items == null){
            System.exit(12);
        }
        return items;
    }

    private static void saveObjectToFile(String filename, Object object) {
        File outFile = null;
        FileWriter writer;
        try {
            outFile = new File(filename);
            if (outFile.createNewFile()) {
                System.err.println("Output file " + outFile.getName() + " created." );
            } else {
                System.err.println("Output file " + outFile.getName() + " already exists.");
            }
        } catch (IOException e) {
            e.printStackTrace();
            System.exit(30);
        }
        System.err.println("Results saved to file " + outFile.getName() + ".");
        try {
            writer = new FileWriter(outFile);
            writer.write(object.toString());
            writer.close();
        } catch (IOException e) {
            e.printStackTrace();
            System.exit(31);
        }
    }

    private static Object getObjectFromJSON(JSONObject object, String key){
        Object content = null;
        try{
            content = object.get(key);
        }catch(JSONException e){
            e.printStackTrace();
            System.exit(17);
        }

        return content;
    }



    public static boolean isInteger(String string) {
        try {
            Integer.valueOf(string);
            return true;
        } catch (NumberFormatException e) {
            return false;
        }
    }

    private static String getFirstIdFromSearch(String question, String token){
        String searchUrl = "https://api.discogs.com/database/search?q=" + question + "&token=" + token;
        String response = sendGet(searchUrl);
        JSONObject json = parseJSON(response);
        JSONArray results = null;

        try{
            results = json.getJSONArray("results");
        }catch(JSONException e){
            e.printStackTrace();
            System.exit(11);
        }

        if(results == null){
            System.exit(12);
        }

        return getObjectFromJSON( (JSONObject) results.get(0), "id").toString();

    }

    public static void main(String[] args){

        final String discogsUrl = "https://api.discogs.com/artists//";
        String url;
        String callback = "?callback=callbackname";
        String token = "olovhcugjeNlcSthaeDMWPuMFMLUoAmXJydyKgML";
        String id;
        
        if(args.length<1){
            System.err.println("No command-line arguments. First one must be name or group ID");
            System.exit(40);
        }

        if(isInteger(args[0])){
            id = args[0];
            System.err.println("Searching by ID: " + args[0]);
        }else{
            String wholeArgs = String.join("-", args);
            System.err.println("Searching by name: " + wholeArgs);
            id = getFirstIdFromSearch(wholeArgs, token);
        }

        url = discogsUrl + id + callback;

        String mainResponse = sendGet(url);
        JSONObject mainGroup = parseJSON(mainResponse);
        JSONObject mainGroupData = (JSONObject) getObjectFromJSON(mainGroup, "data");
        String mainGroupName = getObjectFromJSON(mainGroupData, "name").toString();
        System.err.println(mainGroupName + ":");

        List<Group> groupsList = new ArrayList<>();
        JSONArray mainMembers = getItemsFromData(mainGroup, "members");
        for(int i=0; i<mainMembers.length(); i++){ //wszyscy czlonkowie zespolu
           JSONObject member = (JSONObject) mainMembers.get(i);
            System.err.println("\t" + getObjectFromJSON(member, "name").toString());
            String memberId = getObjectFromJSON(member, "id").toString();
            String memberResponse = sendGet(discogsUrl + memberId + callback);
            JSONObject memberJson = parseJSON(memberResponse);
            JSONArray groups = getItemsFromData(memberJson, "groups");  //grupy w ktorych gral konkretny czlonek
            for(int j=0; j<groups.length(); j++){
                JSONObject group = (JSONObject) groups.get(j);
                String groupName = getObjectFromJSON(group, "name").toString();
                boolean found = false;

                for (Group value : groupsList) {
                    if (value.name.equals(groupName)) {
                        found = true;
                        value.addMember(getObjectFromJSON(member, "name").toString()); //jesli juz taki jest to add zostanie zignorowane
                        break;
                    }
                }

                if(!found && !getObjectFromJSON(group, "name").toString().equals(mainGroupName)) { //jesli takiej grupy nie bylo na liscie to dodaj ja i dodaj membera do tego zoespolu
                    groupsList.add(new Group(getObjectFromJSON(group, "name").toString()));
                    groupsList.get(groupsList.size()-1).addMember(getObjectFromJSON(member, "name").toString());
                }
            }
        }

        JSONArray cases = new JSONArray();
        for(Group g : groupsList){
            if(g.getMembers().size()<2) {
                continue;
            }
            JSONObject groupOut = new JSONObject();
            JSONArray membersOut = new JSONArray();
            for(String name: g.getMembers()){
                membersOut.put(name);
            }
            groupOut.put("members", membersOut);
            groupOut.put("name", g.getName());
            System.out.println(groupOut);
            cases.put(groupOut);
        }
        saveObjectToFile("output.json", cases);
        System.exit(0);
    }
}
