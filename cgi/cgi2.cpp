// #include "../Headers/mlp_request.hpp"

void clients_base::get_CgiEnv(ConfigInfo& config, std::string& file)
{
    this->cgiEnv["REQUEST_METHOD"] = this->request_method;
    this->cgiEnv["REQUEST_URI"] = this->request_url;
    this->cgiEnv["SERVER_PROTOCOL"] = this->request_protocol;
    this->cgiEnv["REDIRECT_STATUS"] = "200";
    this->cgiEnv["SCRIPT_FILENAME"] = file;
    this->cgiEnv["HTTP_COOKIE"] = this->map["Cookie"];
    this->cgiEnv["GATEWAY_INTERFACE"] = "CGI/1.1";
    this->cgiEnv["SERVER_NAME"] = config.get_m_servers()[this->server_id].host;
    this->cgiEnv["SERVER_PORT"] = config.get_m_servers()[this->server_id].s_port;
    this->cgiEnv["QUERY_STRING"]= this->query;
    if (this->request_method == "POST")
    {
        this->cgiEnv["CONTENT_TYPE"] = this->map["Content-Type"];
        this->cgiEnv["CONTENT_LENGTH"] = this->map["Content-Length"];
    }
}

bool clients_base::call_Cgi(ConfigInfo& config, std::string& file)
{
    if (!this->executed)
    {   
        this->runfile = file;
        this->executed = true;
        std::string cgi_Path;
        for(size_t i = 0; i < config.get_m_servers()[this->server_id].s_locations[this->id_match_loc].s_cgi_pass.size(); i++)
        {
            if (this->extension.substr(1) == config.get_m_servers()[this->server_id].s_locations[this->id_match_loc].s_cgi_pass[i].first)
                cgi_Path = config.get_m_servers()[this->server_id].s_locations[this->id_match_loc].s_cgi_pass[i].second;
        }
        if (access(cgi_Path.c_str(), X_OK) == -1)
        {
            this->exitStatus = 500;
            this->ready = true;
            return false;
        }
        this->get_CgiEnv(config, file);
        this->start = clock();
        this->pid = fork();
        if (this->pid == -1)
        {
            this->exitStatus = 500;
            this->ready = true;
            return false;
        }
        if (this->pid == 0)
        {
            std::string cmd = "echo $$ > childpid" + this->m_id + " && " + cgi_Path + " " + file + " > " + "cgi" + this->m_id + "\0";
            char **av = new char*[4];
            av[0] = new char[2];
            av[1] = new char[2];
            av[2] = new char[cmd.length()];
            av[3] = NULL;
            strcpy(av[1], "-c");
            strcpy(av[2], cmd.c_str());
            strcpy(av[0], "sh");
            
            //convert map env variables to char**
            char **envp = new char*[this->cgiEnv.size() + 1];
            int i = 0;
            for (std::map<std::string, std::string>::iterator it = this->cgiEnv.begin(); it != this->cgiEnv.end(); it++)
            {
                envp[i] = new char[it->first.length() + it->second.length() + 2];
                strcpy(envp[i], (it->first + "=" + it->second).c_str());
                i++;
            }
            envp[i] = NULL;
           
            if (execve("/bin/sh", av, envp) == -1)
            {
                for (int j = 0; j < i; j++)
                    delete[] envp[j];
                delete[] envp;
                delete[] av;
                exit(EXIT_FAILURE);
            }
        }
    }
    
    if (waitpid(this->pid, &this->status, WNOHANG) > 0)
    {
        std::remove(("childpid" + this->m_id).c_str());
        if (WEXITSTATUS(this->status) && WEXITSTATUS(this->status) == EXIT_FAILURE)
        {
            this->exitStatus = 500;
            this->ready = true;
            return false;
        }
        std::ifstream tmp1(("cgi" + this->m_id).c_str(), std::ifstream::in);
        if (tmp1.is_open())
        {
            struct stat sb;
            if (stat(("cgi" + this->m_id).c_str(), &sb) == 0)
            {
                this->file_len = sb.st_size;
                this->inf_cgi = new std::ifstream(("cgi" + this->m_id).c_str(), std::ifstream::in);
            }
        }
        this->whilecgi = true;
        this->exitStatus = 0;
        this->ready = true;
    }
    else
    {
        clock_t end = clock();
        clock_t res = (end - this->start) / CLOCKS_PER_SEC;
        if (res > 30)
        {
            std::string filename = "childpid" + this->m_id;
            std::ifstream fd(filename.c_str());
            std::string line;
            std::getline(fd, line);
            fd.close();
            std::remove(filename.c_str());
            std::remove(("cgi" + this->m_id).c_str());
            pid_t pid = std::atoi(line.c_str());
            if (pid != 0)
            {
                if (kill(pid, SIGTERM) == 0)
                {
                    this->exitStatus = 508;
                    return false;
                }
            }
            this->exitStatus = 508;
            return false;
        }
    }
    this->exitStatus = 0;
    this->ready = true;
    return true;
}


bool clients_base::POST_cgi(ConfigInfo& config, std::string& file)
{
    if (!this->executed)
    {
        this->executed = true;
        std::string cgi_Path;
        for(size_t i = 0; i < config.get_m_servers()[this->server_id].s_locations[this->id_match_loc].s_cgi_pass.size(); i++)
        {
            if (this->extension.substr(1) == config.get_m_servers()[this->server_id].s_locations[this->id_match_loc].s_cgi_pass[i].first)
                cgi_Path = config.get_m_servers()[this->server_id].s_locations[this->id_match_loc].s_cgi_pass[i].second;
        }
        if (access(cgi_Path.c_str(), X_OK) == -1)
        {
            this->exitStatus = 500;
            this->ready = true;
            return false;
        }
        this->get_CgiEnv(config, file);
        this->pid = fork();
        if (this->pid == -1)
        {
            this->exitStatus = 500;
            this->ready = true;
            return false;
        }
        if (this->pid == 0)
        {
            std::string cmd = "< " + this->upFpath + " " + cgi_Path + " " + file + " > " + "cgi" + this->m_id + "\0";
            char **av = new char*[4];
            av[0] = new char[2];
            av[1] = new char[2];
            av[2] = new char[cmd.length()];
            av[3] = NULL;
            strcpy(av[0], "sh");
            strcpy(av[1], "-c");
            strcpy(av[2], cmd.c_str());

            char **envp = new char*[this->cgiEnv.size() + 1];
            int i = 0;
            for (std::map<std::string, std::string>::iterator it = this->cgiEnv.begin(); it != this->cgiEnv.end(); it++)
            {
                envp[i] = new char[it->first.length() + it->second.length() + 2];
                strcpy(envp[i], (it->first + "=" + it->second).c_str());
                i++;
            }
            envp[i] = NULL;
            
            if (execve("/bin/sh", av, envp) == -1)
            {
                for (int j = 0; j < i; j++)
                    delete[] envp[j];
                delete[] envp;
                delete[] av;
                exit(EXIT_FAILURE);
            }
        }
    }
    if (waitpid(this->pid, &this->status, WNOHANG) > 0)
    {
        std::ifstream tmp1(("cgi" + this->m_id).c_str(), std::ifstream::in);
        if (tmp1.is_open())
        {
            struct stat sb;
            if (stat(("cgi" + this->m_id).c_str(), &sb) == 0)
            {
                this->file_len = sb.st_size;
                this->inf_cgi = new std::ifstream(("cgi" + this->m_id).c_str(), std::ifstream::in);
            }
        }
        if (WEXITSTATUS(this->status) && WEXITSTATUS(this->status) == EXIT_FAILURE)
        {
            this->exitStatus = 500;
            this->ready = true;
            return false;
        }
        this->whilecgi = true;
        this->exitStatus = 0;
        this->ready = true;
    }
    this->exitStatus = 0;
    this->ready = true;
    return true;
}