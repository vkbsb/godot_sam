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
#include "SuperAnimCommon.h"


class SuperAnimData : public Resource, public SuperAnim::SuperAnimMainDef {
    OBJ_TYPE(SuperAnimData, Resource);
public:

    bool LoadSuperAnimMainDef(const String &theSuperAnimFile);

    SuperAnimData(){

    }
};


class SuperAnimDataFormatLoader : public ResourceFormatLoader{
public:
    virtual RES load(const String &p_path,const String& p_original_path="");
	virtual void get_recognized_extensions(List<String> *p_extensions) const;
	virtual bool handles_type(const String& p_type) const;
	virtual String get_resource_type(const String &p_path) const;
};

#endif /* defined(__GodotEditing__SuperAnimData__) */
