#ifndef __FWRENDEROGE_COMPOSITOR_SAOCOMPOSITORCHAINMANAGER_HPP
#define __FWRENDEROGE_COMPOSITOR_SAOCOMPOSITORCHAINMANAGER_HPP


// used to view the content of the Ogre Compositor Chain
#include <OGRE/OgreCompositorChain.h>
#include <OGRE/OgreCompositorInstance.h>
#include <OGRE/OgreCompositor.h>
#include <OGRE/OgrePrerequisites.h>
#include <OGRE/OgreCompositorManager.h>
#include <OGRE/OgreHardwarePixelBuffer.h>
#include <OGRE/OgreTexture.h>
#include <OGRE/OgreTextureManager.h>
#include <OGRE/OgreTechnique.h>
#include <OGRE/OgrePass.h>


#include <fwCore/BaseObject.hpp>
#include <fwRenderOgre/config.hpp>
#include <vector>

namespace Ogre
{
class CompositorManager;
class Viewport;
}

namespace fwRenderOgre // on s'insère ici
{
    class FWRENDEROGRE_CLASS_API SaoCompositorChainManager : public ::fwCore::BaseObject
    {
    public:
        fwCoreClassDefinitionsWithFactoryMacro ((SaoCompositorChainManager)(::fwRenderOgre::SaoCompositorChainManager),(()),
                                                new SaoCompositorChainManager);
        fwCoreAllowSharedFromThis(); // permet de générer un pointeur vers la classe souhaitée en la castant correctement

        typedef std::string SaoCompositorIdType; // un compositor ne sera ici qu'une chaine de caractère correspondant à son nom (pas de booléen car toujours activé lorsque la chaine est choisie)
        typedef std::vector<SaoCompositorIdType> SaoCompositorChainType; // liste des

        FWRENDEROGRE_API SaoCompositorChainManager();
        FWRENDEROGRE_API SaoCompositorChainManager(::Ogre::Viewport* ogreViewport);// Chaine de compositor attachée au viewport Ogre

        FWRENDEROGRE_API void setOgreViewport(::Ogre::Viewport* viewport);

        // pas sur -> bouton = enable?
        FWRENDEROGRE_API void setSaoState(bool state);

        // fonction to change the value of the sampling radius
        FWRENDEROGRE_API void setSaoRadius(double newRadius);

        // get the radius
        FWRENDEROGRE_API double getSaoRadius();

        // fonction to change the number of samples
        FWRENDEROGRE_API void setSaoSamples(int newSamplesNumber);

        // get the Samples number
        FWRENDEROGRE_API int getSaoSamples();



        // inline function of the end

        FWRENDEROGRE_API SaoCompositorChainType getSaoCompositorChain();

        // Here we add the class derivated from the Ogre Listener
        class FWRENDEROGRE_CLASS_API SaoListener : public ::Ogre::CompositorInstance::Listener
        {
        public:
            SaoListener():m_VP(nullptr),m_Parent(nullptr){}
            SaoListener(::Ogre::Viewport* vp,SaoCompositorChainManager* saoChainManager):m_VP(vp),m_Parent(saoChainManager){}
            ~SaoListener(){}

            SaoCompositorChainManager* getParent(){return m_Parent;}


            void notifyMaterialRender(::Ogre::uint32 pass_id, ::Ogre::MaterialPtr& mat)
                // method called before a render_target operation involving a material to set dynamically the material parameters
            {
                // Test with the two separated compositor

                /*

                //§ When entering in this method, the Mip Map compositor has finished its work

                if (mat.get()->getName() == ::Ogre::MaterialManager::getSingletonPtr()->getByName("AO_mat").get()->getName())
                    std::cout << "Mat Ao_mat appelé" << std::endl;

//                if (pass_id == 1)
//                {
                // no way to do a if with the material name

                std::cout << "No? " << std::endl;
                std::cout << mat.get()->getName() << std::endl;


                // ---------------------------------------------------
                //  Try to change the content of the Second Compositor
                // ---------------------------------------------------


                // get a pointer on the precedent compositor


                ::Ogre::CompositorChain* compChain =
                    ::Ogre::CompositorManager::getSingletonPtr()->getCompositorChain(m_VP);
                ::Ogre::CompositorChain::InstanceIterator compIter = compChain->getCompositors();


    //                ::Ogre::Compositor *MipMap,*Test;
                ::Ogre::TexturePtr mip0,mip1,mip2,mip3,mip4,mip5,mip6,mip7,mip8, rt0;
                while( compIter.hasMoreElements())
                {
                    ::Ogre::CompositorInstance* targetComp = compIter.getNext();
                    if (targetComp->getCompositor()->getName() == "MipMap")
                    {
                        mip0 = targetComp->getTextureInstance("mip0",0);
                        mip1 = targetComp->getTextureInstance("mip1",0);
                        mip2 = targetComp->getTextureInstance("mip2",0);
                        mip3 = targetComp->getTextureInstance("mip3",0);
                        mip4 = targetComp->getTextureInstance("mip4",0);
                        mip5 = targetComp->getTextureInstance("mip5",0);
                        mip6 = targetComp->getTextureInstance("mip6",0);
                        mip7 = targetComp->getTextureInstance("mip7",0);
                        mip8 = targetComp->getTextureInstance("mip8",0);
                    }
                    if (targetComp->getCompositor()->getName() == "AO_Samples")
                    {
                        rt0 = targetComp->getTextureInstance("rt0",0);
                    }
                }


                // ---------------------------------------------------
                //  Copy the content of mip0,mip1... in rt0
                // ---------------------------------------------------


                rt0.get()->freeInternalResources();

                rt0.get()->changeGroupOwnership(mip0.get()->getGroup());
                rt0.get()->setWidth(mip0.get()->getWidth());
                rt0.get()->setHeight(mip0.get()->getHeight());
                rt0.get()->setNumMipmaps(8);
                rt0.get()->setFormat(::Ogre::PixelFormat::PF_FLOAT32_R);
                rt0.get()->setUsage(::Ogre::TU_DYNAMIC_WRITE_ONLY_DISCARDABLE);
                rt0.get()->setTextureType(::Ogre::TextureType::TEX_TYPE_2D);

                rt0.get()->createInternalResources();

                // copy the content of the mip textures in the mipmap (rt0)
                // essayer de le faire de manière automatique avec une boucle + fonction qui récupèrerai l'ensemble des textures de MipMap dans un conteneur

                rt0.get()->getBuffer(0,0)->blit(mip0.get()->getBuffer());
                rt0.get()->getBuffer(0,1)->blit(mip1.get()->getBuffer());
                rt0.get()->getBuffer(0,2)->blit(mip2.get()->getBuffer());
                rt0.get()->getBuffer(0,3)->blit(mip3.get()->getBuffer());
                rt0.get()->getBuffer(0,4)->blit(mip4.get()->getBuffer());
                rt0.get()->getBuffer(0,5)->blit(mip5.get()->getBuffer());
                rt0.get()->getBuffer(0,6)->blit(mip6.get()->getBuffer());
                rt0.get()->getBuffer(0,7)->blit(mip7.get()->getBuffer());
                rt0.get()->getBuffer(0,8)->blit(mip8.get()->getBuffer());

                if (pass_id == 1)
                {
                    std::cout << "AO_mat material called" << std::endl;
//                    mat.get()->getTechnique(0)->getPass(0)->getFragmentProgram().get()->setParameter("r",::Ogre::StringConverter::toString(this->getParent()->getSaoRadius()));

                    mat.get()->getTechnique(0)->getPass(0)->getFragmentProgramParameters().get()->setNamedConstant("r",static_cast<float>(this->getParent()->getSaoRadius()));

                    mat.get()->getTechnique(0)->getPass(0)->getFragmentProgramParameters().get()->setNamedConstant("s",this->getParent()->getSaoSamples());

                }

                */


                // test with only one compositor

                // try to go here only when the AO_mat is called
                if (pass_id  == 1)
                {

                    std::cout << "we are in the pass with the identifier 1" << std::endl;

                    ::Ogre::CompositorChain* compChain =
                        ::Ogre::CompositorManager::getSingletonPtr()->getCompositorChain(m_VP);


                    ::Ogre::CompositorInstance * Sao_compositor = compChain->getCompositor("SAO_complet");
                    ::Ogre::TexturePtr mip0,mip1,mip2,mip3,mip4,mip5,mip6,mip7,mip8, rt0;

                    mip0 = Sao_compositor->getTextureInstance("mip0",0);
                    mip1 = Sao_compositor->getTextureInstance("mip1",0);
                    mip2 = Sao_compositor->getTextureInstance("mip2",0);
                    mip3 = Sao_compositor->getTextureInstance("mip3",0);
                    mip4 = Sao_compositor->getTextureInstance("mip4",0);
                    mip5 = Sao_compositor->getTextureInstance("mip5",0);
                    mip6 = Sao_compositor->getTextureInstance("mip6",0);
                    mip7 = Sao_compositor->getTextureInstance("mip7",0);
                    mip8 = Sao_compositor->getTextureInstance("mip8",0);
                    rt0 = Sao_compositor->getTextureInstance("rt0",0);

                    // ---------------------------------------------------
                    //  Copy the content of mip0,mip1... in rt0
                    // ---------------------------------------------------


                    rt0.get()->freeInternalResources();

                    rt0.get()->changeGroupOwnership(mip0.get()->getGroup());
                    rt0.get()->setWidth(mip0.get()->getWidth());
                    rt0.get()->setHeight(mip0.get()->getHeight());
                    rt0.get()->setNumMipmaps(8);
                    rt0.get()->setFormat(::Ogre::PixelFormat::PF_FLOAT32_R);
                    rt0.get()->setUsage(::Ogre::TU_DYNAMIC_WRITE_ONLY_DISCARDABLE);
                    rt0.get()->setTextureType(::Ogre::TextureType::TEX_TYPE_2D);

                    rt0.get()->createInternalResources();

                    // copy the content of the mip textures in the mipmap (rt0)
                    // essayer de le faire de manière automatique avec une boucle + fonction qui récupèrerai l'ensemble des textures de MipMap dans un conteneur

                    rt0.get()->getBuffer(0,0)->blit(mip0.get()->getBuffer());
                    rt0.get()->getBuffer(0,1)->blit(mip1.get()->getBuffer());
                    rt0.get()->getBuffer(0,2)->blit(mip2.get()->getBuffer());
                    rt0.get()->getBuffer(0,3)->blit(mip3.get()->getBuffer());
                    rt0.get()->getBuffer(0,4)->blit(mip4.get()->getBuffer());
                    rt0.get()->getBuffer(0,5)->blit(mip5.get()->getBuffer());
                    rt0.get()->getBuffer(0,6)->blit(mip6.get()->getBuffer());
                    rt0.get()->getBuffer(0,7)->blit(mip7.get()->getBuffer());
                    rt0.get()->getBuffer(0,8)->blit(mip8.get()->getBuffer());

                    // change the sao arguments

                    mat.get()->getTechnique(0)->getPass(0)->getFragmentProgramParameters().get()->setNamedConstant("r",static_cast<float>(this->getParent()->getSaoRadius()));

                    mat.get()->getTechnique(0)->getPass(0)->getFragmentProgramParameters().get()->setNamedConstant("s",this->getParent()->getSaoSamples());


                }


            }

        private:
            ::Ogre::Viewport* m_VP;

            SaoCompositorChainManager* m_Parent;

        };

    private :
        /// Getter for the Ogre CompositorManager
        ::Ogre::CompositorManager* getCompositorManager();

        /// The parent layer's viewport.
        /// The ogre's compositor manager needs it in order to access the right compositor chain.
        ::Ogre::Viewport* m_ogreViewport;

        SaoCompositorChainType m_saoChain;


        // radius
        double m_SaoRadius;
        // samples number
        int m_SaoSamples;

    };

    //-----------------------------------------------------------------------------
    // Inline method(s)

    inline SaoCompositorChainManager::SaoCompositorChainType SaoCompositorChainManager::getSaoCompositorChain()
    {
        return m_saoChain;
    }

    //-----------------------------------------------------------------------------

    inline void SaoCompositorChainManager::setOgreViewport(::Ogre::Viewport* viewport)
    {
        m_ogreViewport = viewport;
    }

    //-----------------------------------------------------------------------------


}// namespace fwRenderOgre

#endif // __FWRENDEROGE_COMPOSITOR_SAOCOMPOSITORCHAINMANAGER_HPP
