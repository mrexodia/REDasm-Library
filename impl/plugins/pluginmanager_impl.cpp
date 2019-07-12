#include "pluginmanager_impl.h"
#include "pluginloader.h"
#include <redasm/support/utils.h>
#include <redasm/context.h>

#ifdef _WIN32
    #include "../libs/dirent/dirent.h"
#else
    #include <dirent.h>
#endif

namespace REDasm {

std::unique_ptr<PluginManager> PluginManagerImpl::m_instance;

void PluginManagerImpl::unload(const PluginInstance *pi)
{
    String id = pi->descriptor->id;
    PluginLoader::unload(pi);
    m_activeplugins.erase(id);
}

const PluginInstance *PluginManagerImpl::load(const String &pluginpath, const char* initname)
{
    PluginInstance pi;

    if(!PluginLoader::load(pluginpath, initname, &pi))
        return nullptr;

    auto it = m_activeplugins.insert({pi.descriptor->id, pi});

    if(pi.descriptor->plugin)
        pi.descriptor->plugin->setInstance(&it.first->second); // Bind Descriptor <-> Plugin

    return &it.first->second;
}

void PluginManagerImpl::iteratePlugins(const char *initname, const PluginManager_Callback &cb)
{
    for(const String& pluginpath : r_ctx->pluginPaths())
    {
        if(this->iteratePlugins(pluginpath.c_str(), initname, cb))
            break;
    }
}

void PluginManagerImpl::unloadAll()
{
    while(!m_activeplugins.empty())
    {
        auto it = m_activeplugins.begin();
        this->unload(&it->second);
    }
}

bool PluginManagerImpl::execute(const String &id, const ArgumentList& args)
{
    const PluginInstance* pi = this->find(id, REDASM_INIT_PLUGIN_NAME);

    if(!pi)
        return false;

    auto exec = PluginLoader::funcT<Callback_PluginExec>(pi->handle, REDASM_EXEC_NAME);

    if(!exec)
        return false;

    bool res = exec(args);
    this->unload(pi);
    return res;
}

bool PluginManagerImpl::iteratePlugins(const char *path, const char *initname, const PluginManagerImpl::PluginManager_Callback &cb)
{
    DIR* dir = opendir(path);

    if(!dir)
        return false;

    struct dirent* entry = nullptr;

    while((entry = readdir(dir)))
    {
        if(!std::strcmp(entry->d_name, ".") || !std::strcmp(entry->d_name, ".."))
            continue;

        if(entry->d_type == DT_DIR) // Recurse folders
        {
            String rpath = Path::create(path, entry->d_name);

            if(this->iteratePlugins(rpath.c_str(), initname, cb))
                return true;

            continue;
        }

        if(!String(entry->d_name).endsWith(SHARED_OBJECT_EXT))
            continue;

        const PluginInstance* pi = nullptr;

        if(!(pi = this->load(Path::create(path, entry->d_name), initname)))
            continue;

        IterateResult res = cb(pi);

        if(res == IterateResult::Done)
        {
            closedir(dir);
            return true;
        }

        if(res == IterateResult::Unload)
            this->unload(pi);
    }

    closedir(dir);
    return false;
}

const PluginInstance *PluginManagerImpl::find(const char *path, const String &id, const char *initname)
{
    DIR* dir = opendir(path);

    if(!dir)
        return nullptr;

    struct dirent* entry = nullptr;
    const PluginInstance* pi = nullptr;

    while(!pi && (entry = readdir(dir)))
    {
        if(!std::strcmp(entry->d_name, ".") || !std::strcmp(entry->d_name, ".."))
            continue;

        String rpath = Path::create(path, entry->d_name);

        if(entry->d_type == DT_DIR) // Recurse folders
        {
            pi = this->find(rpath.c_str(), id, initname);
            continue;
        }

        if(Path::fileNameOnly(entry->d_name) != id)
            continue;

        pi = this->load(rpath, initname);
    }

    closedir(dir);
    return pi;
}

const PluginInstance *PluginManagerImpl::find(const String &id, const char *initname)
{
    for(const String& pluginpath : r_ctx->pluginPaths())
    {
        const PluginInstance* pi = this->find(pluginpath.c_str(), id, initname);

        if(pi)
            return pi;
    }

    return nullptr;
}

} // namespace REDasm
