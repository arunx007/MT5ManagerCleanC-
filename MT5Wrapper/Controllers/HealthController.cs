using Microsoft.AspNetCore.Mvc;
using Microsoft.AspNetCore.Authorization;
using System;

namespace MT5Wrapper.Controllers
{
    [ApiController]
    [Route("")]
    public class HealthController : ControllerBase
    {
        [HttpGet("health")]
        [AllowAnonymous]
        public IActionResult Health()
        {
            return Ok(new
            {
                status = "healthy",
                timestamp = DateTime.UtcNow,
                service = "MT5 Wrapper API",
                version = "1.0.0"
            });
        }

        [HttpGet("api/health")]
        [AllowAnonymous]
        public IActionResult ApiHealth()
        {
            return Ok(new
            {
                status = "healthy",
                timestamp = DateTime.UtcNow,
                service = "MT5 Wrapper API",
                version = "1.0.0",
                endpoints = new[]
                {
                    "/api/auth/manager/login",
                    "/api/auth/client/login",
                    "/api/auth/validate",
                    "/api/users",
                    "/api/orders",
                    "/api/market",
                    "/health"
                }
            });
        }
    }
}