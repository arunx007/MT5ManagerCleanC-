using Microsoft.AspNetCore.Mvc;
using Microsoft.Extensions.Logging;
using Microsoft.Extensions.Configuration;
using System.Threading.Tasks;
// using MT5Wrapper.Tests; // Commented out - Tests namespace is in separate project

namespace MT5Wrapper.Controllers
{
    [ApiController]
    [Route("api/[controller]")]
    public class TestController : ControllerBase
    {
        private readonly ILogger<TestController> _logger;
        private readonly IConfiguration _config;

        public TestController(ILogger<TestController> logger, IConfiguration config)
        {
            _logger = logger;
            _config = config;
        }

        [HttpGet("run")]
        public IActionResult RunTests()
        {
            _logger.LogInformation("🧪 MT5 API Tests - Use MT5Wrapper.Tests project to run tests");

            return Ok(new
            {
                success = true,
                message = "MT5 API is ready! Use the MT5Wrapper.Tests project to run comprehensive tests.",
                timestamp = System.DateTime.UtcNow,
                details = "Tests are available in the MT5Wrapper.Tests project"
            });
        }

        [HttpGet("health")]
        public IActionResult Health()
        {
            return Ok(new
            {
                status = "healthy",
                message = "MT5 Wrapper API is running",
                timestamp = System.DateTime.UtcNow,
                version = "1.0.0"
            });
        }
    }

}