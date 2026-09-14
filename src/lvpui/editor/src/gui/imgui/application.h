# pragma once
# include <imgui.h>
# include <string>
# include <memory>

struct Platform;
struct Renderer;

struct Application
{
public:
    Application(const char* name);
    Application(const char* name, int argc, char** argv);
    ~Application();

    bool Create(int width = -1, int height = -1);

    int Update();

    void SetTitle(const char* title);

    bool Close();
    void Quit();

    const std::string& GetName() const;

    ImFont* DefaultFont() const;
    ImFont* HeaderFont() const;

    ImTextureID LoadTexture(const char* path);
    ImTextureID LoadDynamicTexture(const char* path);
    ImTextureID CreateTexture(const void* data, int width, int height);
    ImTextureID CreateDynamicTexture(const void* data, int width, int height);
    bool        UpdateTexture(ImTextureID text_id, const void* data);
    bool        UpdateDynamicTexture(ImTextureID text_id, const void* data);
    void        DestroyTexture(ImTextureID texture);
    int         GetTextureWidth(ImTextureID texture);
    int         GetTextureHeight(ImTextureID texture);

    virtual void OnStart() {}
    virtual void OnStop() {}
    virtual void PreFrame(float deltaTime) {}
    virtual void OnFrame(float deltaTime) {}

    virtual ImGuiWindowFlags GetWindowFlags() const;

    virtual bool CanClose() { return true; }

private:
    void RecreateFontAtlas();

    void Frame();

    std::string                 m_Name;
    std::string                 m_IniFilename;
    std::unique_ptr<Platform>   m_Platform;
    std::unique_ptr<Renderer>   m_Renderer;
    ImGuiContext*               m_Context = nullptr;
    ImFont*                     m_DefaultFont = nullptr;
    ImFont*                     m_HeaderFont = nullptr;
};

int Main(int argc, char** argv);