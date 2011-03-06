package com.mar.website.server.util;

import org.hibernate.cfg.AnnotationConfiguration;
import org.hibernate.tool.hbm2ddl.SchemaExport;

import com.mar.website.server.model.Game;
import com.mar.website.server.model.Installation;
import com.mar.website.server.model.User;
import com.mar.website.server.model.UserActivation;

public class HibernateHelper {

	public static final void exportDbSchema(String password) {
		AnnotationConfiguration configuration = new AnnotationConfiguration();
        configuration.setProperty("hibernate.dialect", "org.hibernate.dialect.MySQLInnoDBDialect");
        configuration.setProperty("hibernate.connection.driver_class", "com.mysql.jdbc.Driver");
        configuration.setProperty("hibernate.connection.username", "webuser");
        configuration.setProperty("hibernate.connection.password", password);
        configuration.setProperty("hibernate.connection.url", "jdbc:mysql://localhost/directoryserver");
        configuration.setProperty("hibernate.show_sql", "true");
		
        configuration.addPackage("com.mar.website.server.model");
        
        configuration.addAnnotatedClass(User.class);
        configuration.addAnnotatedClass(UserActivation.class);
        configuration.addAnnotatedClass(Installation.class);
        configuration.addAnnotatedClass(Game.class);
		
		SchemaExport schemaExport = new SchemaExport(configuration);
		schemaExport.drop(true, true);
		schemaExport.create(true, true);
	}
}
