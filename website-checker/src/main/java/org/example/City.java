package org.example;

public class City {
    private final String name;
    private final String URLname;
    private final String id;

    public City(String name, String URLname, String id) {
        this.name = name;
        this.URLname = URLname;
        this.id = id;
    }

    public String getName() {
        return name;
    }

    public String getURLname() {
        return URLname;
    }

    public String getId() {
        return id;
    }
}
