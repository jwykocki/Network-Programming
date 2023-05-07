package org.example;

import com.github.stefanbirkner.systemlambda.SystemLambda;
import static org.junit.jupiter.api.Assertions.*;

public class WebCheckerTest {

    private final String html = "text/html";

    @org.junit.jupiter.api.Test
    public void correctWebsiteTest() throws Exception { //all should be correct

        //to rozwiazanie dziala dla nizszych wersji java
        //nie wiem jak mozna sprawdzac status zakonczonego programu w wersjach wyzszych
        int status = SystemLambda.catchSystemExit(() -> WebChecker.main(
                new String[]{"http://th.if.uj.edu.pl/", html, "Jagiellonian University in Krakow"}));
        assertEquals(0, status);
    }

    @org.junit.jupiter.api.Test
    void incorrectAddressTest() throws Exception {

        int status = SystemLambda.catchSystemExit(() -> WebChecker.main(
                new String[]{"http://th.if.uj.edu.p", html, "Jagiellonian University in Krakow"}));
        assertEquals(1, status);
    }

    //TODO: Test with incorrect web code e.g 404
//    @org.junit.jupiter.api.Test
//    void incorrectCodeTest() throws Exception {
//
//        int status = SystemLambda.catchSystemExit(() -> WebChecker.main(
//                new String[]{"http://niematakiejstrony.com", html, "Szukaj"}));
//        assertEquals(2, status);
//    }

    //TODO: Test with incorrect web type (not text\html)
//    @org.junit.jupiter.api.Test
//    void incorrectTypeTest() throws Exception {
//
//        int status = SystemLambda.catchSystemExit(() -> WebChecker.main(
//                new String[]{"http://niematakiejstrony.com", html, "Szukaj"}));
//        assertEquals(3, status);
//    }

    @org.junit.jupiter.api.Test
    void incorrectContentTest() throws Exception {

        int status = SystemLambda.catchSystemExit(() -> WebChecker.main(
                new String[]{"http://th.if.uj.edu.pl/", html, "BadContent"}));
        assertEquals(4, status);
    }





}