/*
 * This file is protected by Copyright. Please refer to the COPYRIGHT file distributed with this 
 * source distribution.
 * 
 * This file is part of REDHAWK Basic Components.
 * 
 * REDHAWK Basic Components is free software: you can redistribute it and/or modify it under the terms of 
 * the GNU Lesser General Public License as published by the Free Software Foundation, either 
 * version 3 of the License, or (at your option) any later version.
 * 
 * REDHAWK Basic Components is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
 * PURPOSE.  See the GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License along with this 
 * program.  If not, see http://www.gnu.org/licenses/.
 */

#include "HardLimit_base.h"

/*******************************************************************************************

    AUTO-GENERATED CODE. DO NOT MODIFY
    
 	Source: HardLimit.spd.xml
 	Generated on: Thu Feb 21 13:40:22 EST 2013
 	Redhawk IDE
 	Version:M.1.8.2
 	Build id: v201211201139RC3

*******************************************************************************************/

/******************************************************************************************

    The following class functions are for the base class for the component class. To
    customize any of these functions, do not modify them here. Instead, overload them
    on the child class

******************************************************************************************/
 
HardLimit_base::HardLimit_base(const char *uuid, const char *label) :
                                     Resource_impl(uuid, label), serviceThread(0) {
    construct();
}

void HardLimit_base::construct()
{
    Resource_impl::_started = false;
    loadProperties();
    serviceThread = 0;
    
    PortableServer::ObjectId_var oid;
    dataDouble_in = new BULKIO_dataDouble_In_i("dataDouble_in", this);
    oid = ossie::corba::RootPOA()->activate_object(dataDouble_in);
    dataDouble_out = new BULKIO_dataDouble_Out_i("dataDouble_out", this);
    oid = ossie::corba::RootPOA()->activate_object(dataDouble_out);

    registerInPort(dataDouble_in);
    registerOutPort(dataDouble_out, dataDouble_out->_this());
}

/*******************************************************************************************
    Framework-level functions
    These functions are generally called by the framework to perform housekeeping.
*******************************************************************************************/
void HardLimit_base::initialize() throw (CF::LifeCycle::InitializeError, CORBA::SystemException)
{
}

void HardLimit_base::start() throw (CORBA::SystemException, CF::Resource::StartError)
{
    boost::mutex::scoped_lock lock(serviceThreadLock);
    if (serviceThread == 0) {
        dataDouble_in->unblock();
        serviceThread = new ProcessThread<HardLimit_base>(this, 0.1);
        serviceThread->start();
    }
    
    if (!Resource_impl::started()) {
    	Resource_impl::start();
    }
}

void HardLimit_base::stop() throw (CORBA::SystemException, CF::Resource::StopError)
{
    boost::mutex::scoped_lock lock(serviceThreadLock);
    // release the child thread (if it exists)
    if (serviceThread != 0) {
        dataDouble_in->block();
        if (!serviceThread->release(2)) {
            throw CF::Resource::StopError(CF::CF_NOTSET, "Processing thread did not die");
        }
        serviceThread = 0;
    }
    
    if (Resource_impl::started()) {
    	Resource_impl::stop();
    }
}

CORBA::Object_ptr HardLimit_base::getPort(const char* _id) throw (CORBA::SystemException, CF::PortSupplier::UnknownPort)
{

    std::map<std::string, Port_Provides_base_impl *>::iterator p_in = inPorts.find(std::string(_id));
    if (p_in != inPorts.end()) {

        if (!strcmp(_id,"dataDouble_in")) {
            BULKIO_dataDouble_In_i *ptr = dynamic_cast<BULKIO_dataDouble_In_i *>(p_in->second);
            if (ptr) {
                return BULKIO::dataDouble::_duplicate(ptr->_this());
            }
        }
    }

    std::map<std::string, CF::Port_var>::iterator p_out = outPorts_var.find(std::string(_id));
    if (p_out != outPorts_var.end()) {
        return CF::Port::_duplicate(p_out->second);
    }

    throw (CF::PortSupplier::UnknownPort());
}

void HardLimit_base::releaseObject() throw (CORBA::SystemException, CF::LifeCycle::ReleaseError)
{
    // This function clears the component running condition so main shuts down everything
    try {
        stop();
    } catch (CF::Resource::StopError& ex) {
        // TODO - this should probably be logged instead of ignored
    }

    // deactivate ports
    releaseInPorts();
    releaseOutPorts();

    delete(dataDouble_in);
    delete(dataDouble_out);
 
    Resource_impl::releaseObject();
}

void HardLimit_base::loadProperties()
{
    addProperty(upper_limit,
                1, 
               "upper_limit",
               "",
               "readwrite",
               "",
               "external",
               "configure");

    addProperty(lower_limit,
                -1, 
               "lower_limit",
               "",
               "readwrite",
               "",
               "external",
               "configure");

}
