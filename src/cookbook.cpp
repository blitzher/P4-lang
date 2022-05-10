#include "epicr.h"

namespace epicr
{

    void generate_cookbook(std::string directory, std::string output_directory)
    {
        std::filesystem::directory_iterator dir_iter(directory);
        std::filesystem::path out_dir(output_directory);
        out_dir = std::filesystem::absolute(out_dir);

        std::string output_file_content = "<html>";

        output_file_content += std::string() +
                               "<head><style>" +
                               "body, div, a{display:flex; place-content:center; align-items:center; flex-direction:column;}\n" +
                               "div {min-width:100vw}" +
                               "a { padding:15px; background: #faf3ee; font-weight:bold; font-size:2em; width:100%}" +
                               "h1 {}" +
                               "</style></head>";

        output_file_content += "<h1>epicR Cookbook of " + directory + "</h1><div>";
        for (const auto &file : dir_iter)
        {
            if (!string_ends_with(file.path().filename(), ".rcp"))
                continue;

            const auto rcp_ret = parse_recipe(file.path().string());
            if (rcp_ret.has_err)
            {
                std::cout << rcp_ret.err;
                continue;
            }
            recipe rcp = rcp_ret.recipe;

            auto vis_ret = visitor::visit_all(&rcp);
            if (vis_ret.has_err)
            {
                std::cout << vis_ret.err;
                continue;
            }

            std::string filename = out_dir / file.path().stem().string();

            switch (clargs.choosen_style)
            {
            case E_OS_FANCY:
                filename += ".html";
                generate_fancy(rcp, filename);
                break;
            case E_OS_BASIC:
                filename += ".md";
                generate_basic(rcp, filename);
                break;
            default:
                break;
            }

            output_file_content += "<a href='" + filename + "'>" + rcp.title + "</a><br>";
        }

        output_file_content += "</div></body></html>";

        std::ofstream output_file(out_dir / "cookbook.html");
        output_file << output_file_content << std::endl;
    }
}