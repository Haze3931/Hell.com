using JigokuTsushin.Net.wwwroot;
using Microsoft.AspNetCore.Mvc;
using Microsoft.AspNetCore.Mvc.RazorPages;
using Microsoft.Extensions.Logging;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace JigokuTsushin.Net.Pages
{
    public class IndexModel : PageModel
    {
        [BindProperty]
        public Correspondence correspondence { get; set; }
        private readonly ILogger<IndexModel> _logger;

        public IndexModel(ILogger<IndexModel> logger)
        {
            _logger = logger;
        }

        public void OnGet()
        {

        }

        public IActionResult OnPost()
        {
            if(ModelState.IsValid == false)
            {
                return Page();
            }
            try
            {
                using (FileStream fs = System.IO.File.Create(@"..\\JigokuTsushin.Net\Correspondance\"
                    + correspondence.Name + ".cr"))
                {
                    byte[] title = new UTF8Encoding(true).GetBytes(correspondence.Name);
                    fs.Write(title, 0, title.Length);
                }
            }
            catch
            {
                return Forbid();
            }
            return Forbid();
        }
    }
}
