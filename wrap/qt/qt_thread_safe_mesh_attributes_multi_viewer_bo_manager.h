/***************************************************************************
* VCGLib                                                            o o     *
* Visual and Computer Graphics Library                            o     o   *
*                                                                _   O  _   *
* Copyright(C) 2004-2014                                           \/)\/    *
* Visual Computing Lab                                            /\/|      *
* ISTI - Italian National Research Council                           |      *
*                                                                    \      *
* All rights reserved.                                                      *
*                                                                           *
* This program is free software; you can redistribute it and/or modify      *
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 2 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
* for more details.                                                         *
*                                                                           *
****************************************************************************/

#ifndef __QT_THREAD_SAFE_RENDERER_H
#define __QT_THREAD_SAFE_RENDERER_H


#include <wrap/qt/qt_thread_safe_memory_info.h>
#include <wrap/qt/qt_thread_safe_texture_names_container.h>
#include <wrap/gl/gl_mesh_attributes_multi_viewer_bo_manager.h>

namespace vcg
{
    template<typename MESH_TYPE,typename UNIQUE_VIEW_ID_TYPE,typename GL_OPTIONS_DERIVED_TYPE = vcg::PerViewPerRenderingModalityGLOptions>
    class QtThreadSafeGLMeshAttributesMultiViewerBOManager : public vcg::NotThreadSafeGLMeshAttributesMultiViewerBOManager<MESH_TYPE,UNIQUE_VIEW_ID_TYPE,GL_OPTIONS_DERIVED_TYPE>
	{
	public:
		QtThreadSafeGLMeshAttributesMultiViewerBOManager(MESH_TYPE& mesh,QtThreadSafeMemoryInfo& gpumeminfo,size_t perbatchtriangles)
			:vcg::NotThreadSafeGLMeshAttributesMultiViewerBOManager<MESH_TYPE,UNIQUE_VIEW_ID_TYPE,GL_OPTIONS_DERIVED_TYPE>(mesh,gpumeminfo,perbatchtriangles),_lock(QReadWriteLock::Recursive)
		{
		}
		
		~QtThreadSafeGLMeshAttributesMultiViewerBOManager() {}

                void meshAttributesUpdated(bool hasmeshconnectivitychanged,const vcg::GLMeshAttributesInfo::RendAtts& changedrendatts)
		{
			QWriteLocker locker(&_lock);
			vcg::NotThreadSafeGLMeshAttributesMultiViewerBOManager<MESH_TYPE,UNIQUE_VIEW_ID_TYPE,GL_OPTIONS_DERIVED_TYPE>::meshAttributesUpdated(hasmeshconnectivitychanged,changedrendatts);
		}

                bool getPerViewInfo(UNIQUE_VIEW_ID_TYPE viewid,vcg::GLMeshAttributesInfo::PRIMITIVE_MODALITY_MASK* mask,vcg::GLMeshAttributesInfo::RendAtts* rendatts,vcg::PerViewPerRenderingModalityGLOptions*  opts)
        {
			QReadLocker locker(&_lock);
			return vcg::NotThreadSafeGLMeshAttributesMultiViewerBOManager<MESH_TYPE,UNIQUE_VIEW_ID_TYPE,GL_OPTIONS_DERIVED_TYPE>::getPerViewInfo(viewid,mask,rendatts,opts);
		}
		
        void setPerViewInfo(UNIQUE_VIEW_ID_TYPE viewid,vcg::GLMeshAttributesInfo::PRIMITIVE_MODALITY_MASK pm,const vcg::GLMeshAttributesInfo::RendAtts& reqatts)
        {
            QWriteLocker locker(&_lock);
            vcg::NotThreadSafeGLMeshAttributesMultiViewerBOManager<MESH_TYPE,UNIQUE_VIEW_ID_TYPE,GL_OPTIONS_DERIVED_TYPE>::setPerViewInfo(viewid,pm,reqatts);
        }

		void removeView(UNIQUE_VIEW_ID_TYPE viewid)
		{
            QWriteLocker locker(&_lock);
            vcg::NotThreadSafeGLMeshAttributesMultiViewerBOManager<MESH_TYPE,UNIQUE_VIEW_ID_TYPE,GL_OPTIONS_DERIVED_TYPE>::removeView(viewid);
		}

		void draw(UNIQUE_VIEW_ID_TYPE viewid) const
		{
			QReadLocker locker(&_lock);
			vcg::NotThreadSafeGLMeshAttributesMultiViewerBOManager<MESH_TYPE,UNIQUE_VIEW_ID_TYPE,GL_OPTIONS_DERIVED_TYPE>::draw(viewid,_textids.textId());
		}

		bool isBORenderingAvailable() const
		{
			QReadLocker locker(&_lock);
			return vcg::NotThreadSafeGLMeshAttributesMultiViewerBOManager<MESH_TYPE,UNIQUE_VIEW_ID_TYPE,GL_OPTIONS_DERIVED_TYPE>::isBORenderingAvailable();
		}
		
        bool manageBuffers()
        {
            QWriteLocker locker(&_lock);
            return vcg::NotThreadSafeGLMeshAttributesMultiViewerBOManager<MESH_TYPE,UNIQUE_VIEW_ID_TYPE,GL_OPTIONS_DERIVED_TYPE>::manageBuffers();
        }

        void removeAllViewsAndDeallocateBO()
        {
            QWriteLocker locker(&_lock);
            vcg::NotThreadSafeGLMeshAttributesMultiViewerBOManager<MESH_TYPE,UNIQUE_VIEW_ID_TYPE,GL_OPTIONS_DERIVED_TYPE>::removeAllViews();
            vcg::NotThreadSafeGLMeshAttributesMultiViewerBOManager<MESH_TYPE,UNIQUE_VIEW_ID_TYPE,GL_OPTIONS_DERIVED_TYPE>::manageBuffers();
        }

		inline vcg::QtThreadSafeTextureNamesContainer& textureIDContainer() {return _textids;}

        void setTrMatrix(const vcg::Matrix44<typename MESH_TYPE::ScalarType>& tr)
        {
            QWriteLocker locker(&_lock);
            vcg::NotThreadSafeGLMeshAttributesMultiViewerBOManager<MESH_TYPE,UNIQUE_VIEW_ID_TYPE,GL_OPTIONS_DERIVED_TYPE>::setTrMatrix(tr);
        }

        void setGLOptions(UNIQUE_VIEW_ID_TYPE viewid,const GL_OPTIONS_DERIVED_TYPE& opts)
        {
            QWriteLocker locker(&_lock);
            vcg::NotThreadSafeGLMeshAttributesMultiViewerBOManager<MESH_TYPE,UNIQUE_VIEW_ID_TYPE,GL_OPTIONS_DERIVED_TYPE>::setGLOptions(viewid,opts);
        }

	private:
		mutable QReadWriteLock _lock;
		vcg::QtThreadSafeTextureNamesContainer _textids;
	};
}

#endif