package com.headius.contact;

import javax.ejb.EJBObject;
import java.rmi.RemoteException;

/**
 * This is the remote interface for the Contact bean, which represents a contact
 * entry in the database
 */
public interface Contact extends EJBObject {
    public Integer getID() throws RemoteException;
    public String getFirstName() throws RemoteException;
    public void setFirstName(String fn) throws RemoteException;
    public String getMiddleName() throws RemoteException;
    public void setMiddleName(String mn) throws RemoteException;
    public String getLastName() throws RemoteException;
    public void setLastName(String ln) throws RemoteException;

    /**
     * Returns the full name as First Middle Last
     */
    public String getNameFML() throws RemoteException;
    
    /**
     * Returns the full name as Last, First Middle
     */
    public String getNameLcFM() throws RemoteException;

    public String getTitle() throws RemoteException;
    public void setTitle(String t) throws RemoteException;

    public String getAddress() throws RemoteException;
    public void setAddress(String a) throws RemoteException;

    public String getCity() throws RemoteException;
    public void setCity(String c) throws RemoteException;

    public String getState() throws RemoteException;
    public void setState(String s) throws RemoteException;

    public String getPostalCode() throws RemoteException;
    public void setPostalCode(String pc) throws RemoteException;

    public String getCountry() throws RemoteException;
    public void setCountry(String c) throws RemoteException;

    public String getHomePhone() throws RemoteException;
    public void setHomePhone(String hp) throws RemoteException;

    public String getWorkPhone() throws RemoteException;
    public void setWorkPhone(String wp) throws RemoteException;

    public String getMobilePhone() throws RemoteException;
    public void setMobilePhone(String mp) throws RemoteException;

    public String getHomeEmail() throws RemoteException;
    public void setHomeEmail(String he) throws RemoteException;

    public String getWorkEmail() throws RemoteException;
    public void setWorkEmail(String we) throws RemoteException;

    public String getMobileEmail() throws RemoteException;
    public void setMobileEmail(String me) throws RemoteException;

    public String getICQ() throws RemoteException;
    public void setICQ(String i) throws RemoteException;

    public String getAIM() throws RemoteException;
    public void setAIM(String a) throws RemoteException;

    public String getMSN() throws RemoteException;
    public void setMSN(String m) throws RemoteException;

    public String getYahoo() throws RemoteException;
    public void setYahoo(String y) throws RemoteException;

    public String getHomepage() throws RemoteException;
    public void setHomepage(String h) throws RemoteException;
}
