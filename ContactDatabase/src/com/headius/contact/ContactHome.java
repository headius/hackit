package com.headius.contact;

import javax.ejb.EJBHome;
import javax.ejb.CreateException;
import javax.ejb.FinderException;
import java.util.Collection;
import java.rmi.RemoteException;

/**
 * This is the home interface for the Contact bean
 */
public interface ContactHome extends EJBHome {
    public Contact create(Integer id) throws CreateException, RemoteException;
    
    public Contact findByPrimaryKey(Integer id) throws FinderException, RemoteException;

    public Collection findByLastName(String lastname) throws FinderException, RemoteException;

    public Collection findByEmail(String email) throws FinderException, RemoteException;

    public Collection findAll() throws FinderException, RemoteException;
}
