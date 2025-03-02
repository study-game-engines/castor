﻿/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PassMapComponent_H___
#define ___C3D_PassMapComponent_H___

#include "Castor3D/Material/Texture/TextureModule.hpp"
#include "Castor3D/Shader/Shaders/SdwModule.hpp"

#include "Castor3D/Material/Pass/Component/PassComponent.hpp"
#include "Castor3D/Material/Texture/TextureConfiguration.hpp"
#include "Castor3D/Material/Texture/TextureSourceInfo.hpp"

namespace castor3d
{
	using TextureSourceMap = std::unordered_map< TextureSourceInfo
		, PassTextureConfig
		, TextureSourceInfoHasher >;
	using TextureSourceMapVT = TextureSourceMap::value_type;
	using TextureSourceSet = std::unordered_set< TextureSourceInfo, TextureSourceInfoHasher >;

	class PassMapComponentPlugin
		: public PassComponentPlugin
	{
	public:
		C3D_API explicit PassMapComponentPlugin( PassComponentRegister const & passComponent
			, UpdateComponent pupdateComponent = nullptr )
			: PassComponentPlugin{ passComponent, pupdateComponent }
		{
		}

		C3D_API bool writeTextureConfig( TextureConfiguration const & configuration
			, castor::String const & tabs
			, castor::StringStream & file )const override;

	private:
		virtual bool doWriteTextureConfig( TextureConfiguration const & configuration
			, uint32_t mask
			, castor::String const & tabs
			, castor::StringStream & file )const
		{
			return true;
		}
	};

	struct PassMapComponent
		: public PassComponent
	{
		/**
		*\name
		*	Construction / Destruction.
		*/
		/**@{*/
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	pass			The parent pass.
		 *\param[in]	type			The component type.
		 *\param[in]	textureFlags	The texture flags.
		 *\param[in]	deps			The components this one depends on.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	pass			Le sous-maillage pass.
		 *\param[in]	type			Le type de composant.
		 *\param[in]	textureFlags	Les indicateurs de texture.
		 *\param[in]	deps			Les composants dont celui-ci dépend.
		 */
		C3D_API PassMapComponent( Pass & pass
			, castor::String type
			, TextureFlags textureFlags
			, castor::StringArray deps = {} );
		/**@}*/
		/**
		*\~english
		*\name
		*	Getters.
		*\~french
		*\name
		*	Accesseurs.
		*/
		/**@{*/
		PassComponentTextureFlag getTextureFlags()const noexcept
		{
			return m_textureFlags;
		}

		C3D_API void fillConfig( TextureConfiguration & config
			, ConfigurationVisitorBase & vis )const override;

		void fillChannel( TextureConfiguration & configuration
			, uint32_t mask )const
		{
			getPlugin().fillTextureConfiguration( configuration, mask );
		}
		/**@}*/

	private:
		virtual void doFillConfig( TextureConfiguration & configuration
			, ConfigurationVisitorBase & vis )const
		{
		}

	private:
		PassComponentTextureFlag m_textureFlags;
	};
}

#endif
