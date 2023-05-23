package org.example;
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

//TODO: opakować metodą wyciaganie danych z JSONa
//TODO: zrobic wyszukiwanie zespolu nazwa
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

    private static void saveListToFile(String filename, List<JSONObject> list) {
        File outFile = null;
        FileWriter writer = null;
        try {
            outFile = new File(filename);
            if (outFile.createNewFile()) {
                System.err.println("Output file created: " + outFile.getName());
            } else {
                System.err.println("Output file already exists.");
            }
        } catch (IOException e) {
            e.printStackTrace();
            System.exit(30);
        }
        try {
            writer = new FileWriter(outFile);
            for (JSONObject obj : list) {
                writer.write(obj.toString());
                writer.write("\r\n");
            }
            writer.close();
        } catch (IOException e) {
            e.printStackTrace();
            System.exit(31);
        }
    }


    private static final String discogsUrl = "https://api.discogs.com/artists//";

    public static void main(String[] args){

        int id = 18839;
        String callback = "?callback=callbackname";
        String url = discogsUrl + id + callback;

        String mainResponse = sendGet(url);
        JSONObject mainGroup = parseJSON(mainResponse);
        String mainGroupName = ((JSONObject)mainGroup.get("data")).get("name").toString();
        System.err.println("Członkowie " + mainGroupName + " to: ");

        List<Group> groupsList = new ArrayList<>();
        JSONArray mainMembers = getItemsFromData(mainGroup, "members");
        for(int i=0; i<mainMembers.length(); i++){ //wszyscy czlonkowie zespolu
           JSONObject member = (JSONObject) mainMembers.get(i);
            System.err.println(member.get("name"));
            String memberId = member.get("id").toString();
            String memberResponse = sendGet(discogsUrl + memberId + callback);
            JSONObject memberJson = parseJSON(memberResponse);
            JSONArray groups = getItemsFromData(memberJson, "groups");  //grupy w ktorych gral konkretny czlonek
            for(int j=0; j<groups.length(); j++){
                JSONObject group = (JSONObject) groups.get(j);
                String groupName = group.get("name").toString();
                boolean found = false;

                for (Group value : groupsList) {
                    if (value.name.equals(groupName)) {
                        found = true;
                        value.addMember(member.get("name").toString()); //jesli juz taki jest to add zostanie zignorowane
                        break;
                    }
                }

                if(!found && !group.get("name").toString().equals(mainGroupName)) { //jesli takiej grupy nie bylo na liscie to dodaj ja i dodaj membera do tego zoespolu
                    groupsList.add(new Group(group.get("name").toString()));
                    groupsList.get(groupsList.size()-1).addMember(member.get("name").toString());
                }
            }
        }

        System.err.println("-------------------------------------------");
        ArrayList<JSONObject> groupsOut = new ArrayList<>();
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
            groupsOut.add(groupOut);
        }
        saveListToFile("list.txt", groupsOut);
        System.exit(0);
    }
}
