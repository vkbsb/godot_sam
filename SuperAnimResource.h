//
//  SuperAnimData.h
//  GodotEditing
//
//  Created by vamsi krishna on 24/02/15.
//  Copyright (c) 2015 vamsi krishna. All rights reserved.
//

#ifndef __GodotEditing__SuperAnimData__
#define __GodotEditing__SuperAnimData__

#include "core/Resource.h"
#include "core/io/resource_loader.h"
#include "core/io/resource_saver.h"
#include "SuperAnimCommon.h"

/*
 * We have to define a class that inherits from Resource class to be able to
 * add support for a different file format.
 */
class SuperAnimData : public Resource, public SuperAnim::SuperAnimMainDef {
    OBJ_TYPE(SuperAnimData, Resource);
public:

    bool LoadSuperAnimMainDef(const String &theSuperAnimFile);

    SuperAnimData(){

    }
};

/* This class is written to make godot understand that .sam files
 * are to be loaded using the SuperAnimData class.
 */
class SuperAnimDataFormatLoader : public ResourceFormatLoader{
public:
    //This function is called by godot to load the resources which are handled by this loader.
    //if a resource is already loaded once, this function is not called again.
    virtual RES load(const String &p_path,const String& p_original_path="");

    //this function specifies which file extensions this loader can load.
	virtual void get_recognized_extensions(List<String> *p_extensions) const;

    //this function is to be specified to load the Resource Type defined by SuperAnimData.
    //this is required to ensure that the engine is able to load when the scene is loaded.
	virtual bool handles_type(const String& p_type) const;

    //NOT sure where this is used.
	virtual String get_resource_type(const String &p_path) const;
};


class SuperAnimDataFormatSaver : public ResourceFormatSaver {
public:

    virtual Error save(const String &p_path,const RES& p_resource,uint32_t p_flags=0);
    virtual void get_recognized_extensions(const RES& p_resource,List<String> *p_extensions) const;
    virtual bool recognize(const RES& p_resource) const;

};

#endif /* defined(__GodotEditing__SuperAnimData__) */
