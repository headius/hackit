package com.headius.contact;

import javax.ejb.EntityBean;
import javax.ejb.EntityContext;
import java.rmi.RemoteException;

/**
 * This is the bean for the Contact ejb
 */
public class ContactBean implements EntityBean {
    EntityContext ctx;

    public Integer id;
    public String firstName;
    public String middleName;
    public String lastName;
    public String title;
    public String address;
    public String city;
    public String state;
    public String postalCode;
    public String country;
    public String homePhone;
    public String workPhone;
    public String mobilePhone;
    public String homeEmail;
    public String workEmail;
    public String mobileEmail;
    public String icq;
    public String msn;
    public String aim;
    public String yahoo;
    public String homepage;

    public Integer ejbCreate(Integer theID) {
	id = theID;
	return null;
    }

    public void ejbPostCreate(Integer theID) {
    }

    public Integer getID() {
        return id;
    }

    public String getFirstName() {
	return firstName;
    }

    public void setFirstName(String fn) {
	firstName = fn;
    }

    public String getMiddleName() {
	return middleName;
    }

    public void setMiddleName(String mn) {
	middleName = mn;
    }

    public String getLastName() {
	return lastName;
    }

    public void setLastName(String ln) {
	lastName = ln;
    }

    public String getNameFML() {
        StringBuffer name = new StringBuffer();
	name.append(firstName);
        if (middleName != null && middleName.length() != 0) {
            name.append(" ");
            name.append(middleName);
        }
        if (lastName != null && lastName.length() != 0) {
            name.append(" ");
            name.append(lastName);
        }

        return name.toString();
    }

    public String getNameLcFM() {
        StringBuffer name = new StringBuffer();
        name.append(lastName);
        if (firstName != null && firstName.length() != 0) {
            name.append(", ");
            name.append(firstName);
        }
        if (middleName != null && middleName.length() != 0) {
            name.append(" ");
            name.append(middleName);
        }

        return name.toString();
    }


    public String getTitle() {
	return title;
    }

    public void setTitle(String t) {
	title = t;
    }

    public String getAddress() {
	return address;
    }

    public void setAddress(String a) {
	address = a;
    }

    public String getCity() {
	return city;
    }

    public void setCity(String c) {
	city = c;
    }

    public String getState() {
	return state;
    }

    public void setState(String s) {
	state = s;
    }

    public String getPostalCode() {
	return postalCode;
    }

    public void setPostalCode(String pc) {
	postalCode = pc;
    }

    public String getCountry() {
	return country;
    }
    
    public void setCountry(String c) {
	country = c;
    }
    
    public String getHomePhone() {
	return homePhone;
    }

    public void setHomePhone(String hp) {
	homePhone = hp;
    }

    public String getWorkPhone() {
	return workPhone;
    }

    public void setWorkPhone(String wp) {
	workPhone = wp;
    }

    public String getMobilePhone() {
	return mobilePhone;
    }

    public void setMobilePhone(String mp) {
	mobilePhone = mp;
    }

    public String getHomeEmail() {
	return homeEmail;
    }

    public void setHomeEmail(String he) {
	homeEmail = he;
    }

    public String getWorkEmail() {
	return workEmail;
    }

    public void setWorkEmail(String we) {
	workEmail = we;
    }

    public String getMobileEmail() {
	return mobileEmail;
    }

    public void setMobileEmail(String me) {
	mobileEmail = me;
    }

    public String getICQ() {
	return icq;
    }

    public void setICQ(String i) {
	icq = i;
    }

    public String getAIM() {
	return aim;
    }

    public void setAIM(String a) {
	aim = a;
    }

    public String getMSN() {
	return msn;
    }

    public void setMSN(String m) {
	msn = m;
    }

    public String getYahoo() {
	return yahoo;
    }

    public void setYahoo(String y) {
	yahoo = y;
    }

    public String getHomepage() {
	return homepage;
    }

    public void setHomepage(String h) {
	homepage = h;
    }

    public void setEntityContext(EntityContext e) {
	ctx = e;
    }

    public void unsetEntityContext() {
        ctx = null;
    }

    public void ejbActivate() {
    }

    public void ejbPassivate() {
    }

    public void ejbLoad() {
    }

    public void ejbStore() {
    }

    public void ejbRemove() {
    }
}
